// Moderated LMS interactive console application.
// The interface delegates core academic workflows to WorkflowService so that
// role menus cannot bypass the same validation used by automated tests.
#include <sqlite3.h>
#include <iostream>
#include <iomanip>
#include <optional>
#include <string>
#include <vector>
#include <limits>
#include <filesystem>
#include <chrono>
#include <ctime>
#include "repositories/Database.h"
#include "repositories/UserRepository.h"
#include "services/PasswordHasher.h"
#include "services/AuthenticationService.h"
#include "services/WorkflowService.h"

using lms::domain::UserRole;
using lms::repositories::StoredUser;

namespace {
std::string prompt(const std::string& label) {
    std::cout << label;
    std::string value;
    std::getline(std::cin, value);
    return value;
}
int number(const std::string& label, int minimum = 0) {
    for (;;) {
        const auto s = prompt(label);
        try { std::size_t used=0; int n=std::stoi(s,&used); if(used==s.size()&&n>=minimum)return n; }
        catch (...) {}
        std::cout << "Enter a whole number >= " << minimum << ".\n";
    }
}
double realNumber(const std::string& label, double minimum = 0.0) {
    for (;;) {
        const auto s=prompt(label);
        try { std::size_t used=0; double n=std::stod(s,&used); if(used==s.size()&&n>=minimum)return n; }
        catch (...) {}
        std::cout << "Enter a valid number >= " << minimum << ".\n";
    }
}
bool execute(sqlite3* db,const std::string& sql,const std::vector<std::string>& values={}) {
    sqlite3_stmt* st=nullptr;
    if(sqlite3_prepare_v2(db,sql.c_str(),-1,&st,nullptr)!=SQLITE_OK){std::cout<<"Database error: "<<sqlite3_errmsg(db)<<'\n';return false;}
    for(int i=0;i<(int)values.size();++i)sqlite3_bind_text(st,i+1,values[i].c_str(),-1,SQLITE_TRANSIENT);
    const int rc=sqlite3_step(st); const bool ok=rc==SQLITE_DONE; if(!ok)std::cout<<"Database error: "<<sqlite3_errmsg(db)<<'\n'; sqlite3_finalize(st); return ok;
}
void rows(sqlite3* db,const std::string& sql,const std::vector<std::string>& values={}) {
    sqlite3_stmt* st=nullptr; if(sqlite3_prepare_v2(db,sql.c_str(),-1,&st,nullptr)!=SQLITE_OK){std::cout<<"Database error: "<<sqlite3_errmsg(db)<<'\n';return;}
    for(int i=0;i<(int)values.size();++i)sqlite3_bind_text(st,i+1,values[i].c_str(),-1,SQLITE_TRANSIENT);
    const int cols=sqlite3_column_count(st); bool any=false;
    while(sqlite3_step(st)==SQLITE_ROW){any=true;for(int i=0;i<cols;++i){if(i)std::cout<<" | ";const auto* p=sqlite3_column_text(st,i);std::cout<<(p?reinterpret_cast<const char*>(p):"NULL");}std::cout<<'\n';}
    if(!any) std::cout<<"No records found.\n";
    sqlite3_finalize(st);
}
bool exists(sqlite3* db,const std::string& sql,int id){sqlite3_stmt* st=nullptr;if(sqlite3_prepare_v2(db,sql.c_str(),-1,&st,nullptr)!=SQLITE_OK)return false;sqlite3_bind_int(st,1,id);bool yes=sqlite3_step(st)==SQLITE_ROW;sqlite3_finalize(st);return yes;}
std::string nowLocal(){const auto t=std::time(nullptr);std::tm tm{};
#ifdef _WIN32
 localtime_s(&tm,&t);
#else
 localtime_r(&t,&tm);
#endif
 char b[20];std::strftime(b,sizeof(b),"%Y-%m-%d %H:%M:%S",&tm);return b;}
void bootstrap(lms::repositories::UserRepository& users,sqlite3* db){sqlite3_stmt* st=nullptr;int count=0;if(sqlite3_prepare_v2(db,"SELECT COUNT(*) FROM users WHERE role='Administrator'",-1,&st,nullptr)==SQLITE_OK&&sqlite3_step(st)==SQLITE_ROW)count=sqlite3_column_int(st,0);if(st)sqlite3_finalize(st);if(count)return;
 std::cout<<"First run: create the initial Administrator account.\n";for(;;){auto u=prompt("Administrator username: ");auto p=prompt("Administrator password: ");int id=0;if(!u.empty()&&p.size()>=8&&users.createUser(u,p,UserRole::Administrator,std::nullopt,id)){std::cout<<"Administrator created (ID "<<id<<").\n";return;}std::cout<<"Creation failed. Use a non-empty username and password of at least 8 characters. "<<users.lastError()<<'\n';}}
void adminMenu(sqlite3* db,lms::repositories::UserRepository& users){for(;;){std::cout<<"\nADMINISTRATOR\n1 Create account\n2 List accounts\n3 Activate/deactivate account\n4 Create department\n5 Create course\n6 Enrol student\n7 List departments and courses\n0 Logout\n";int c=number("Choose: ");if(!c)return;
 if(c==1){auto u=prompt("Username: "),p=prompt("Temporary password (8+ characters): ");std::cout<<"Role: 1 Administrator, 2 Lecturer, 3 HoD, 4 Student\n";int r=number("Role: ",1);if(r>4){std::cout<<"Invalid role.\n";continue;}std::string d=prompt("Department ID (blank for none): ");std::optional<int> dept;if(!d.empty()){try{dept=std::stoi(d);}catch(...){std::cout<<"Invalid department ID.\n";continue;}}UserRole role=r==1?UserRole::Administrator:r==2?UserRole::Lecturer:r==3?UserRole::HeadOfDepartment:UserRole::Student;int id=0;if(users.createUser(u,p,role,dept,id))std::cout<<"Account created. ID: "<<id<<'\n';else std::cout<<users.lastError()<<'\n';}
 else if(c==2)rows(db,"SELECT user_id,username,role,COALESCE(department_id,''),is_active FROM users ORDER BY user_id");
 else if(c==3){int id=number("User ID: ",1);bool active=prompt("Activate account? (y/n): ")=="y";if(users.setUserActive(id,active))std::cout<<"Account status updated.\n";else std::cout<<users.lastError()<<'\n';}
 else if(c==4){auto n=prompt("Department name: ");if(!n.empty()&&execute(db,"INSERT INTO departments(name) VALUES(?)",{n}))std::cout<<"Department created.\n";}
 else if(c==5){rows(db,"SELECT department_id,name FROM departments ORDER BY department_id");int d=number("Department ID: ",1);auto code=prompt("Course code: "),title=prompt("Course title: ");if(execute(db,"INSERT INTO courses(department_id,course_code,title) VALUES(?,?,?)",{std::to_string(d),code,title}))std::cout<<"Course created.\n";}
 else if(c==6){int s=number("Student user ID: ",1),course=number("Course ID: ",1);if(!exists(db,"SELECT 1 FROM users WHERE user_id=? AND role='Student' AND is_active=1",s)){std::cout<<"Active student not found.\n";continue;}if(execute(db,"INSERT INTO enrollments(student_id,course_id,enrolled_at) VALUES(?,?,CURRENT_TIMESTAMP)",{std::to_string(s),std::to_string(course)}))std::cout<<"Student enrolled.\n";}
 else if(c==7){rows(db,"SELECT department_id,name FROM departments ORDER BY department_id");rows(db,"SELECT course_id,course_code,title,department_id FROM courses ORDER BY course_id");}else std::cout<<"Unknown option.\n";}}
void lecturerMenu(sqlite3* db,const StoredUser& user,lms::services::WorkflowService& flow){for(;;){std::cout<<"\nLECTURER\n1 Create assignment\n2 Grant individual extension\n3 Grant whole-class extension\n4 View current submissions\n5 Mark current submission\n6 View my assignments\n0 Logout\n";int c=number("Choose: ");if(!c)return;
 if(c==1){rows(db,"SELECT course_id,course_code,title FROM courses ORDER BY course_id");int course=number("Course ID: ",1);auto title=prompt("Assignment title: "),desc=prompt("Description: "),due=prompt("Deadline (YYYY-MM-DD HH:MM:SS): ");double max=realNumber("Maximum mark: ",0.01);int id=0;if(flow.createAssignment(course,user.userId,title,desc,due,max,id))std::cout<<"Assignment created. ID: "<<id<<'\n';else std::cout<<flow.lastError()<<'\n';}
 else if(c==2||c==3){int aid=number("Assignment ID: ",1);std::string student,deadline,reason;if(c==2)student=std::to_string(number("Student user ID: ",1));deadline=prompt("Revised deadline (YYYY-MM-DD HH:MM:SS): ");reason=prompt("Reason: ");bool ok=c==2?flow.grantIndividualExtension(aid,std::stoi(student),user.userId,deadline,reason):flow.grantClassExtension(aid,user.userId,deadline,reason);std::cout<<(ok?"Extension saved.\n":flow.lastError()+"\n");}
 else if(c==4)rows(db,"SELECT s.submission_id,s.assignment_id,s.student_id,s.version_no,s.file_path,s.submitted_at,s.status FROM submissions s JOIN assignments a ON a.assignment_id=s.assignment_id WHERE a.lecturer_id="+std::to_string(user.userId)+" AND s.is_current=1 ORDER BY s.assignment_id,s.student_id");
 else if(c==5){int sid=number("Submission ID: ",1);double mark=realNumber("Raw mark: ");auto feedback=prompt("Feedback: "),reason=prompt("Reason for mark/audit: ");int mid=0;if(flow.recordMark(sid,user.userId,mark,feedback,reason,mid))std::cout<<"Mark recorded (ID "<<mid<<") and sent for HoD moderation.\n";else std::cout<<flow.lastError()<<'\n';}
 else if(c==6)rows(db,"SELECT assignment_id,course_id,title,deadline,max_mark FROM assignments WHERE lecturer_id="+std::to_string(user.userId)+" ORDER BY assignment_id");else std::cout<<"Unknown option.\n";}}
void hodMenu(sqlite3* db,const StoredUser& user,lms::services::WorkflowService& flow){for(;;){std::cout<<"\nHEAD OF DEPARTMENT\n1 View marks awaiting moderation\n2 Approve mark\n3 Request changes\n4 Release approved mark\n5 View moderation history\n0 Logout\n";int c=number("Choose: ");if(!c)return;
 if(c==1)rows(db,"SELECT m.mark_id,s.assignment_id,s.student_id,m.value,m.status,m.feedback FROM marks m JOIN submissions s ON s.submission_id=m.submission_id WHERE m.status='AwaitingModeration' ORDER BY m.mark_id");
 else if(c==2||c==3){int id=number("Mark ID: ",1);auto comments=prompt("Moderation comments: ");bool ok=flow.moderateMark(id,user.userId,c==2,comments);std::cout<<(ok?(c==2?"Mark approved. Use option 4 to release it.\n":"Changes requested; lecturer may revise the mark.\n"):flow.lastError()+"\n");}
 else if(c==4){int id=number("Approved mark ID: ",1);bool ok=flow.releaseMark(id,user.userId);std::cout<<(ok?"Mark released to student.\n":flow.lastError()+"\n");}
 else if(c==5)rows(db,"SELECT moderation_id,mark_id,hod_user_id,decision,decision_at,comments FROM moderation_records ORDER BY moderation_id DESC");else std::cout<<"Unknown option.\n";}}
void studentMenu(sqlite3* db,const StoredUser& user,lms::services::WorkflowService& flow){for(;;){std::cout<<"\nSTUDENT\n1 View assignments and deadlines\n2 Submit/resubmit assignment\n3 View released marks and feedback\n4 View my submission history\n0 Logout\n";int c=number("Choose: ");if(!c)return;
 if(c==1){std::vector<lms::services::StudentAssignment> a;if(flow.listAssignmentsForStudent(user.userId,a)){if(a.empty())std::cout<<"No assignments for your enrolled courses.\n";for(const auto& x:a)std::cout<<x.assignmentId<<" | "<<x.title<<" | deadline "<<x.originalDeadline<<" | your deadline "<<x.effectiveDeadline<<" | max "<<x.maximumMark<<(x.deadlineExtended?" (extended)":"")<<'\n';}else std::cout<<flow.lastError()<<'\n';}
 else if(c==2){int aid=number("Assignment ID: ",1);auto path=prompt("Full path to your submission file: ");if(path.empty()||!std::filesystem::is_regular_file(path)){std::cout<<"That file does not exist or is not a regular file.\n";continue;}int sid=0;if(flow.submit(aid,user.userId,path,nowLocal(),sid))std::cout<<"Submission recorded. ID: "<<sid<<". The system stores the path; keep the file available.\n";else std::cout<<flow.lastError()<<'\n';}
 else if(c==3)rows(db,"SELECT a.title,m.value,a.max_mark,m.feedback FROM marks m JOIN submissions s ON s.submission_id=m.submission_id JOIN assignments a ON a.assignment_id=s.assignment_id WHERE s.student_id="+std::to_string(user.userId)+" AND m.status='Released' AND s.is_current=1");
 else if(c==4)rows(db,"SELECT s.submission_id,s.assignment_id,s.version_no,s.file_path,s.submitted_at,s.status,s.is_current FROM submissions s WHERE s.student_id="+std::to_string(user.userId)+" ORDER BY s.assignment_id,s.version_no");else std::cout<<"Unknown option.\n";}}
}
int main(){std::cout<<"====================================\n       MODERATED LMS (OFFLINE)\n====================================\n";lms::repositories::Database db("moderated_lms.db");if(!db.initialize()){std::cerr<<db.lastError()<<'\n';return 1;}try{lms::services::PasswordHasher hasher;lms::repositories::UserRepository users(db.connection(),hasher);lms::services::AuthenticationService auth(users,hasher);lms::services::WorkflowService workflow(db.connection());bootstrap(users,db.connection());for(;;){std::cout<<"\n1 Login\n0 Exit\n";int c=number("Choose: ");if(!c)break;auto u=prompt("Username: "),p=prompt("Password: ");auto user=auth.login(u,p);if(!user){std::cout<<"Login failed. "<<auth.lastError()<<'\n';continue;}std::cout<<"Welcome, "<<user->username<<".\n";switch(user->role){case UserRole::Administrator:adminMenu(db.connection(),users);break;case UserRole::Lecturer:lecturerMenu(db.connection(),*user,workflow);break;case UserRole::HeadOfDepartment:hodMenu(db.connection(),*user,workflow);break;case UserRole::Student:studentMenu(db.connection(),*user,workflow);break;}}std::cout<<"Goodbye.\n";}catch(const std::exception& e){std::cerr<<"Application error: "<<e.what()<<'\n';return 1;}return 0;}
