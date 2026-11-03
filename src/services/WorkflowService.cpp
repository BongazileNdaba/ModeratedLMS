#include "services/WorkflowService.h"

#include <algorithm>
#include <cmath>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <utility>

namespace lms::services {
namespace {
class Statement {
public:
    Statement(sqlite3* db, const char* sql) : stmt_(nullptr) {
        rc_ = sqlite3_prepare_v2(db, sql, -1, &stmt_, nullptr);
    }
    ~Statement() { if (stmt_) sqlite3_finalize(stmt_); }
    sqlite3_stmt* get() const { return stmt_; }
    int result() const { return rc_; }
    void close() { if (stmt_) { sqlite3_finalize(stmt_); stmt_ = nullptr; } }
private:
    sqlite3_stmt* stmt_;
    int rc_;
};

bool validDateTime(const std::string& value) {
    // ISO date or ISO timestamp; lexical comparison is safe for this format.
    return value.size() >= 10 && value[4] == '-' && value[7] == '-';
}

bool hasColumn(sqlite3* db, const char* table, const char* column) {
    std::string sql = std::string("PRAGMA table_info(") + table + ")";
    sqlite3_stmt* statement = nullptr;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &statement, nullptr) != SQLITE_OK)
        return false;
    bool found = false;
    while (sqlite3_step(statement) == SQLITE_ROW) {
        const auto* name = sqlite3_column_text(statement, 1);
        if (name && std::string(reinterpret_cast<const char*>(name)) == column) {
            found = true;
            break;
        }
    }
    sqlite3_finalize(statement);
    return found;
}
}

WorkflowService::WorkflowService(sqlite3* connection) : db_(connection) {
    // Additive migrations preserve existing records and are safe to run repeatedly.
    if (!hasColumn(db_, "marks", "raw_mark"))
        execute("ALTER TABLE marks ADD COLUMN raw_mark REAL;");
    if (!hasColumn(db_, "marks", "penalty"))
        execute("ALTER TABLE marks ADD COLUMN penalty REAL NOT NULL DEFAULT 0;");
    if (!hasColumn(db_, "marks", "final_mark"))
        execute("ALTER TABLE marks ADD COLUMN final_mark REAL;");
    execute(R"SQL(
        CREATE TABLE IF NOT EXISTS whole_class_extensions (
            extension_id INTEGER PRIMARY KEY AUTOINCREMENT,
            assignment_id INTEGER NOT NULL,
            revised_deadline TEXT NOT NULL,
            approved_by_lecturer_id INTEGER NOT NULL,
            approved_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
            reason TEXT NOT NULL,
            FOREIGN KEY (assignment_id) REFERENCES assignments(assignment_id),
            FOREIGN KEY (approved_by_lecturer_id) REFERENCES users(user_id)
        );
    )SQL");
}

bool WorkflowService::execute(const char* sql) {
    char* error = nullptr;
    int rc = sqlite3_exec(db_, sql, nullptr, nullptr, &error);
    if (rc != SQLITE_OK) {
        lastError_ = error ? error : sqlite3_errmsg(db_);
        sqlite3_free(error);
        return false;
    }
    return true;
}

bool WorkflowService::hasRole(int id, const std::string& role) {
    Statement q(db_, "SELECT 1 FROM users WHERE user_id=? AND role=? AND is_active=1");
    if (q.result()!=SQLITE_OK) return false;
    sqlite3_bind_int(q.get(),1,id);
    sqlite3_bind_text(q.get(),2,role.c_str(),-1,SQLITE_TRANSIENT);
    return sqlite3_step(q.get())==SQLITE_ROW;
}

bool WorkflowService::isEnrolled(int studentId, int courseId) {
    Statement q(db_, "SELECT 1 FROM enrollments WHERE student_id=? AND course_id=?");
    if (q.result()!=SQLITE_OK) return false;
    sqlite3_bind_int(q.get(),1,studentId);
    sqlite3_bind_int(q.get(),2,courseId);
    return sqlite3_step(q.get())==SQLITE_ROW;
}

bool WorkflowService::assignmentDetails(int id, int& course, double& max,
                                        std::string& deadline, int& lecturer) {
    Statement q(db_, "SELECT course_id,max_mark,deadline,lecturer_id FROM assignments WHERE assignment_id=?");
    if (q.result()!=SQLITE_OK) return false;
    sqlite3_bind_int(q.get(),1,id);
    if (sqlite3_step(q.get())!=SQLITE_ROW) return false;
    course=sqlite3_column_int(q.get(),0);
    max=sqlite3_column_double(q.get(),1);
    deadline=reinterpret_cast<const char*>(sqlite3_column_text(q.get(),2));
    lecturer=sqlite3_column_int(q.get(),3);
    return true;
}

std::string WorkflowService::effectiveDeadline(int assignmentId, int studentId,
                                                const std::string& base) {
    Statement q(db_, R"SQL(
        SELECT revised_deadline FROM (
          SELECT revised_deadline, approved_at AS sort_time
          FROM individual_extensions WHERE assignment_id=? AND student_id=?
          UNION ALL
          SELECT revised_deadline, approved_at AS sort_time
          FROM whole_class_extensions WHERE assignment_id=?
        ) ORDER BY sort_time DESC LIMIT 1
    )SQL");
    if (q.result()!=SQLITE_OK) return base;
    sqlite3_bind_int(q.get(),1,assignmentId);
    sqlite3_bind_int(q.get(),2,studentId);
    sqlite3_bind_int(q.get(),3,assignmentId);
    if (sqlite3_step(q.get())==SQLITE_ROW) {
        const auto* p=sqlite3_column_text(q.get(),0);
        if (p) return reinterpret_cast<const char*>(p);
    }
    return base;
}

int WorkflowService::calendarDaysLate(const std::string& due,
                                      const std::string& submitted) {
    if (!validDateTime(due) || !validDateTime(submitted)) return -1;
    std::tm a{}, b{};
    std::istringstream da(due.substr(0,10)), db(submitted.substr(0,10));
    da >> std::get_time(&a,"%Y-%m-%d");
    db >> std::get_time(&b,"%Y-%m-%d");
    if (da.fail() || db.fail()) return -1;
    a.tm_hour=12; b.tm_hour=12; // avoid daylight-saving boundary surprises
    std::time_t ta=std::mktime(&a), tb=std::mktime(&b);
    if (ta==(std::time_t)-1 || tb==(std::time_t)-1) return -1;
    double days=std::difftime(tb,ta)/86400.0;
    return std::max(0,static_cast<int>(std::ceil(days)));
}

bool WorkflowService::createAssignment(int courseId, int lecturerId,
    const std::string& title, const std::string& description,
    const std::string& deadline, double maximumMark, int& assignmentId) {
    if (courseId<=0 || lecturerId<=0 || title.empty() || !validDateTime(deadline) || maximumMark<=0) {
        lastError_="Invalid assignment details."; return false;
    }
    if (!hasRole(lecturerId,"Lecturer")) { lastError_="An active Lecturer account is required."; return false; }
    Statement q(db_,"INSERT INTO assignments(course_id,lecturer_id,title,description,deadline,max_mark,created_at) VALUES(?,?,?,?,?,?,CURRENT_TIMESTAMP)");
    if(q.result()!=SQLITE_OK){lastError_=sqlite3_errmsg(db_);return false;}
    sqlite3_bind_int(q.get(),1,courseId);
    sqlite3_bind_int(q.get(),2,lecturerId);
    sqlite3_bind_text(q.get(),3,title.c_str(),-1,SQLITE_TRANSIENT);
    sqlite3_bind_text(q.get(),4,description.c_str(),-1,SQLITE_TRANSIENT);
    sqlite3_bind_text(q.get(),5,deadline.c_str(),-1,SQLITE_TRANSIENT);
    sqlite3_bind_double(q.get(),6,maximumMark);
    if(sqlite3_step(q.get())!=SQLITE_DONE){lastError_=sqlite3_errmsg(db_);return false;}
    assignmentId=static_cast<int>(sqlite3_last_insert_rowid(db_));
    return true;
}

bool WorkflowService::grantIndividualExtension(int assignmentId,int studentId,
    int lecturerId,const std::string& revised,const std::string& reason) {
    int course=0, owner=0; double max=0; std::string due;
    if(!assignmentDetails(assignmentId,course,max,due,owner)){lastError_="Assignment not found.";return false;}
    if(owner!=lecturerId || !hasRole(lecturerId,"Lecturer")){lastError_="Only the assignment lecturer may grant this extension.";return false;}
    if(!isEnrolled(studentId,course)){lastError_="Student is not enrolled in this course.";return false;}
    if(!validDateTime(revised)||revised<=due||reason.empty()){lastError_="Extension must have a later valid deadline and a reason.";return false;}
    Statement q(db_,"INSERT INTO individual_extensions(assignment_id,student_id,revised_deadline,approved_by_lecturer_id,approved_at,reason) VALUES(?,?,?,?,CURRENT_TIMESTAMP,?) ON CONFLICT(assignment_id,student_id) DO UPDATE SET revised_deadline=excluded.revised_deadline,approved_by_lecturer_id=excluded.approved_by_lecturer_id,approved_at=CURRENT_TIMESTAMP,reason=excluded.reason");
    if(q.result()!=SQLITE_OK){lastError_=sqlite3_errmsg(db_);return false;}
    sqlite3_bind_int(q.get(),1,assignmentId); sqlite3_bind_int(q.get(),2,studentId);
    sqlite3_bind_text(q.get(),3,revised.c_str(),-1,SQLITE_TRANSIENT);
    sqlite3_bind_int(q.get(),4,lecturerId); sqlite3_bind_text(q.get(),5,reason.c_str(),-1,SQLITE_TRANSIENT);
    if(sqlite3_step(q.get())!=SQLITE_DONE){lastError_=sqlite3_errmsg(db_);return false;} return true;
}

bool WorkflowService::grantClassExtension(int assignmentId,int lecturerId,
    const std::string& revised,const std::string& reason) {
    int course=0,owner=0; double max=0; std::string due;
    if(!assignmentDetails(assignmentId,course,max,due,owner)){lastError_="Assignment not found.";return false;}
    if(owner!=lecturerId||!hasRole(lecturerId,"Lecturer")){lastError_="Only the assignment lecturer may grant this extension.";return false;}
    if(!validDateTime(revised)||revised<=due||reason.empty()){lastError_="Extension must have a later valid deadline and a reason.";return false;}
    Statement q(db_,"INSERT INTO whole_class_extensions(assignment_id,revised_deadline,approved_by_lecturer_id,reason) VALUES(?,?,?,?)");
    if(q.result()!=SQLITE_OK){lastError_=sqlite3_errmsg(db_);return false;}
    sqlite3_bind_int(q.get(),1,assignmentId);sqlite3_bind_text(q.get(),2,revised.c_str(),-1,SQLITE_TRANSIENT);
    sqlite3_bind_int(q.get(),3,lecturerId);sqlite3_bind_text(q.get(),4,reason.c_str(),-1,SQLITE_TRANSIENT);
    if(sqlite3_step(q.get())!=SQLITE_DONE){lastError_=sqlite3_errmsg(db_);return false;} return true;
}

bool WorkflowService::listAssignmentsForStudent(
    int studentId, std::vector<StudentAssignment>& assignments) {
    assignments.clear();
    if (!hasRole(studentId, "Student")) {
        lastError_ = "An active Student account is required.";
        return false;
    }

    // Only show assignments belonging to courses in which this student is enrolled.
    Statement q(db_, R"SQL(
        SELECT a.assignment_id, a.title, a.description, a.deadline, a.max_mark
        FROM assignments a
        JOIN enrollments e ON e.course_id = a.course_id
        WHERE e.student_id = ?
        ORDER BY a.deadline, a.assignment_id
    )SQL");
    if (q.result() != SQLITE_OK) {
        lastError_ = sqlite3_errmsg(db_);
        return false;
    }
    sqlite3_bind_int(q.get(), 1, studentId);

    int rc = SQLITE_ROW;
    while ((rc = sqlite3_step(q.get())) == SQLITE_ROW) {
        StudentAssignment item;
        item.assignmentId = sqlite3_column_int(q.get(), 0);
        const auto* title = sqlite3_column_text(q.get(), 1);
        const auto* description = sqlite3_column_text(q.get(), 2);
        const auto* deadline = sqlite3_column_text(q.get(), 3);
        item.title = title ? reinterpret_cast<const char*>(title) : "";
        item.description = description ? reinterpret_cast<const char*>(description) : "";
        item.originalDeadline = deadline ? reinterpret_cast<const char*>(deadline) : "";
        item.maximumMark = sqlite3_column_double(q.get(), 4);
        item.effectiveDeadline = effectiveDeadline(
            item.assignmentId, studentId, item.originalDeadline);
        item.deadlineExtended = item.effectiveDeadline != item.originalDeadline;
        assignments.push_back(std::move(item));
    }
    if (rc != SQLITE_DONE) {
        lastError_ = sqlite3_errmsg(db_);
        assignments.clear();
        return false;
    }
    return true;
}

bool WorkflowService::submit(int assignmentId,int studentId,const std::string& file,
    const std::string& submittedAt,int& submissionId) {
    int course=0,lecturer=0; double max=0; std::string due;
    if(!assignmentDetails(assignmentId,course,max,due,lecturer)){lastError_="Assignment not found.";return false;}
    if(!hasRole(studentId,"Student")||!isEnrolled(studentId,course)){lastError_="Active enrolled student account required.";return false;}
    if(file.empty()||!validDateTime(submittedAt)){lastError_="A file path and valid submission time are required.";return false;}
    const std::string effective=effectiveDeadline(assignmentId,studentId,due);
    const int late=calendarDaysLate(effective,submittedAt);
    if(late<0){lastError_="Invalid deadline or submission date.";return false;}
    if(!execute("BEGIN IMMEDIATE"))return false;
    Statement version(db_,"SELECT COALESCE(MAX(version_no),0)+1 FROM submissions WHERE assignment_id=? AND student_id=?");
    sqlite3_bind_int(version.get(),1,assignmentId);sqlite3_bind_int(version.get(),2,studentId);
    if(sqlite3_step(version.get())!=SQLITE_ROW){execute("ROLLBACK");lastError_=sqlite3_errmsg(db_);return false;}
    int n=sqlite3_column_int(version.get(),0);
    version.close();
    Statement old(db_,"UPDATE submissions SET is_current=0 WHERE assignment_id=? AND student_id=? AND is_current=1");
    sqlite3_bind_int(old.get(),1,assignmentId);sqlite3_bind_int(old.get(),2,studentId);
    if(sqlite3_step(old.get())!=SQLITE_DONE){execute("ROLLBACK");lastError_=sqlite3_errmsg(db_);return false;}
    Statement ins(db_,"INSERT INTO submissions(assignment_id,student_id,version_no,file_path,submitted_at,status,is_current) VALUES(?,?,?,?,?,?,1)");
    sqlite3_bind_int(ins.get(),1,assignmentId);sqlite3_bind_int(ins.get(),2,studentId);sqlite3_bind_int(ins.get(),3,n);
    sqlite3_bind_text(ins.get(),4,file.c_str(),-1,SQLITE_TRANSIENT);sqlite3_bind_text(ins.get(),5,submittedAt.c_str(),-1,SQLITE_TRANSIENT);
    sqlite3_bind_text(ins.get(),6,late>3?"LateZero":"Submitted",-1,SQLITE_STATIC);
    if(sqlite3_step(ins.get())!=SQLITE_DONE){execute("ROLLBACK");lastError_=sqlite3_errmsg(db_);return false;}
    submissionId=static_cast<int>(sqlite3_last_insert_rowid(db_));
    if(!execute("COMMIT")){execute("ROLLBACK");return false;} return true;
}

bool WorkflowService::recordMark(int submissionId,int lecturerId,double raw,
    const std::string& feedback,const std::string& reason,int& markId) {
    if(raw<0||!std::isfinite(raw)){lastError_="Mark must be a non-negative finite number.";return false;}
    Statement q(db_,R"SQL(SELECT s.assignment_id,s.student_id,s.submitted_at,a.max_mark,a.deadline,a.lecturer_id
      FROM submissions s JOIN assignments a ON a.assignment_id=s.assignment_id
      WHERE s.submission_id=? AND s.is_current=1)SQL");
    sqlite3_bind_int(q.get(),1,submissionId);
    if(sqlite3_step(q.get())!=SQLITE_ROW){lastError_="Current submission not found.";return false;}
    int assignment=sqlite3_column_int(q.get(),0),student=sqlite3_column_int(q.get(),1);
    std::string submitted=reinterpret_cast<const char*>(sqlite3_column_text(q.get(),2));
    double maximum=sqlite3_column_double(q.get(),3);
    std::string due=reinterpret_cast<const char*>(sqlite3_column_text(q.get(),4));
    int owner=sqlite3_column_int(q.get(),5);
    q.close();
    if(owner!=lecturerId||!hasRole(lecturerId,"Lecturer")){lastError_="Only the assignment lecturer may mark this submission.";return false;}
    if(raw>maximum){lastError_="Raw mark cannot exceed the assignment maximum.";return false;}
    due=effectiveDeadline(assignment,student,due);
    int late=calendarDaysLate(due,submitted);
    double penalty=late>3?maximum:std::min(maximum,10.0*maximum/100.0*late);
    double finalMark=late>3?0.0:std::max(0.0,raw-penalty);
    // Preserve the previous value in the audit trail when a mark is revised.
    bool hadPrevious = false;
    double previousValue = 0.0;
    Statement previous(db_, "SELECT final_mark FROM marks WHERE submission_id=?");
    sqlite3_bind_int(previous.get(), 1, submissionId);
    if (sqlite3_step(previous.get()) == SQLITE_ROW) {
        hadPrevious = true;
        previousValue = sqlite3_column_double(previous.get(), 0);
    }
    previous.close();
    if(!execute("BEGIN IMMEDIATE"))return false;
    Statement ins(db_,"INSERT INTO marks(submission_id,value,feedback,status,raw_mark,penalty,final_mark) VALUES(?,?,?,'AwaitingModeration',?,?,?) ON CONFLICT(submission_id) DO UPDATE SET value=excluded.value,feedback=excluded.feedback,status='AwaitingModeration',raw_mark=excluded.raw_mark,penalty=excluded.penalty,final_mark=excluded.final_mark");
    sqlite3_bind_int(ins.get(),1,submissionId);sqlite3_bind_double(ins.get(),2,finalMark);
    sqlite3_bind_text(ins.get(),3,feedback.c_str(),-1,SQLITE_TRANSIENT);sqlite3_bind_double(ins.get(),4,raw);
    sqlite3_bind_double(ins.get(),5,penalty);sqlite3_bind_double(ins.get(),6,finalMark);
    if(sqlite3_step(ins.get())!=SQLITE_DONE){execute("ROLLBACK");lastError_=sqlite3_errmsg(db_);return false;}
    Statement find(db_,"SELECT mark_id FROM marks WHERE submission_id=?");
    sqlite3_bind_int(find.get(),1,submissionId);
    if(sqlite3_step(find.get())!=SQLITE_ROW){execute("ROLLBACK");lastError_="Could not retrieve saved mark.";return false;}
    markId=sqlite3_column_int(find.get(),0);
    find.close();
    Statement audit(db_,"INSERT INTO mark_audit_log(mark_id,changed_by_user_id,changed_at,old_value,new_value,reason) VALUES(?,?,CURRENT_TIMESTAMP,?,?,?)");
    sqlite3_bind_int(audit.get(),1,markId);sqlite3_bind_int(audit.get(),2,lecturerId);
    if(hadPrevious) sqlite3_bind_double(audit.get(),3,previousValue); else sqlite3_bind_null(audit.get(),3);
    sqlite3_bind_double(audit.get(),4,finalMark);
    sqlite3_bind_text(audit.get(),5,reason.c_str(),-1,SQLITE_TRANSIENT);
    if(sqlite3_step(audit.get())!=SQLITE_DONE){execute("ROLLBACK");lastError_=sqlite3_errmsg(db_);return false;}
    if(!execute("COMMIT")){execute("ROLLBACK");return false;}return true;
}

bool WorkflowService::moderateMark(int markId,int hodId,bool approve,const std::string& comments) {
    if(!hasRole(hodId,"HoD")){lastError_="Active HoD account required.";return false;}
    Statement check(db_,"SELECT status FROM marks WHERE mark_id=?");
    sqlite3_bind_int(check.get(),1,markId);
    if(sqlite3_step(check.get())!=SQLITE_ROW){lastError_="Mark not found.";return false;}
    std::string status=reinterpret_cast<const char*>(sqlite3_column_text(check.get(),0));
    check.close();
    if(status!="AwaitingModeration"){lastError_="Only marks awaiting moderation can be reviewed.";return false;}
    if(!execute("BEGIN IMMEDIATE"))return false;
    Statement update(db_,"UPDATE marks SET status=? WHERE mark_id=?");
    sqlite3_bind_text(update.get(),1,approve?"Approved":"Draft",-1,SQLITE_STATIC);sqlite3_bind_int(update.get(),2,markId);
    if(sqlite3_step(update.get())!=SQLITE_DONE){execute("ROLLBACK");lastError_=sqlite3_errmsg(db_);return false;}
    Statement rec(db_,"INSERT INTO moderation_records(mark_id,hod_user_id,decision,decision_at,comments) VALUES(?,?,?,CURRENT_TIMESTAMP,?)");
    sqlite3_bind_int(rec.get(),1,markId);sqlite3_bind_int(rec.get(),2,hodId);
    sqlite3_bind_text(rec.get(),3,approve?"Approved":"ChangesRequested",-1,SQLITE_STATIC);
    sqlite3_bind_text(rec.get(),4,comments.c_str(),-1,SQLITE_TRANSIENT);
    if(sqlite3_step(rec.get())!=SQLITE_DONE){execute("ROLLBACK");lastError_=sqlite3_errmsg(db_);return false;}
    if(!execute("COMMIT")){execute("ROLLBACK");return false;}return true;
}

bool WorkflowService::releaseMark(int markId,int hodId) {
    if(!hasRole(hodId,"HoD")){lastError_="Active HoD account required.";return false;}
    Statement q(db_,"UPDATE marks SET status='Released' WHERE mark_id=? AND status='Approved'");
    sqlite3_bind_int(q.get(),1,markId);
    if(sqlite3_step(q.get())!=SQLITE_DONE){lastError_=sqlite3_errmsg(db_);return false;}
    if(sqlite3_changes(db_)!=1){lastError_="Only an approved mark can be released.";return false;}return true;
}
const std::string& WorkflowService::lastError() const { return lastError_; }
} // namespace lms::services
