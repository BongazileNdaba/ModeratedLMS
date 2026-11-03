#include <gtest/gtest.h>
#include <sqlite3.h>
#include <cmath>
#include <memory>
#include <limits>
#include <optional>
#include <string>
#include <vector>

#include "repositories/Database.h"
#include "repositories/UserRepository.h"
#include "services/PasswordHasher.h"
#include "services/WorkflowService.h"

// Test fixture: each test receives a fresh in-memory SQLite database and
// separate accounts for the administrator, lecturer, students and HoD.
class WorkflowTests : public ::testing::Test {
protected:
    lms::repositories::Database database{":memory:"};
    lms::services::PasswordHasher hasher;
    std::unique_ptr<lms::repositories::UserRepository> users;
    std::unique_ptr<lms::services::WorkflowService> workflow;
    int admin=0, lecturer=0, student=0, student2=0, hod=0, course=0, assignment=0;

    void SetUp() override {
        ASSERT_TRUE(database.initialize()) << database.lastError();
        users = std::make_unique<lms::repositories::UserRepository>(database.connection(), hasher);
        ASSERT_TRUE(users->createUser("test_admin", "TestPassword1!", lms::domain::UserRole::Administrator, std::nullopt, admin));
        ASSERT_TRUE(users->createUser("test_lecturer", "TestPassword1!", lms::domain::UserRole::Lecturer, std::nullopt, lecturer));
        ASSERT_TRUE(users->createUser("test_student", "TestPassword1!", lms::domain::UserRole::Student, std::nullopt, student));
        ASSERT_TRUE(users->createUser("test_student2", "TestPassword1!", lms::domain::UserRole::Student, std::nullopt, student2));
        ASSERT_TRUE(users->createUser("test_hod", "TestPassword1!", lms::domain::UserRole::HeadOfDepartment, std::nullopt, hod));
        ASSERT_EQ(sqlite3_exec(database.connection(),
            "INSERT INTO departments(name) VALUES('Science');"
            "INSERT INTO courses(department_id,course_code,title) VALUES(1,'SCI101','Science');",
            nullptr,nullptr,nullptr), SQLITE_OK);
        course=1;
        ASSERT_EQ(sqlite3_exec(database.connection(),
            "INSERT INTO enrollments(student_id,course_id,enrolled_at) VALUES(3,1,CURRENT_TIMESTAMP);"
            "INSERT INTO enrollments(student_id,course_id,enrolled_at) VALUES(4,1,CURRENT_TIMESTAMP);",
            nullptr,nullptr,nullptr), SQLITE_OK);
        workflow=std::make_unique<lms::services::WorkflowService>(database.connection());
        ASSERT_TRUE(workflow->createAssignment(course,lecturer,"Test assignment","Test",
            "2026-10-10",100.0,assignment)) << workflow->lastError();
    }

    // Read one numeric result from SQLite, for example a row count or mark.
    double number(const std::string& sql, int parameter=0) {
        sqlite3_stmt* q=nullptr;
        if (sqlite3_prepare_v2(database.connection(),sql.c_str(),-1,&q,nullptr)!=SQLITE_OK) return -999999;
        if (parameter) sqlite3_bind_int(q,1,parameter);
        double result = sqlite3_step(q)==SQLITE_ROW ? sqlite3_column_double(q,0) : -999999;
        sqlite3_finalize(q);
        return result;
    }

    std::string stringValue(const std::string& sql, int parameter) {
        sqlite3_stmt* q=nullptr;
        if (sqlite3_prepare_v2(database.connection(),sql.c_str(),-1,&q,nullptr)!=SQLITE_OK) return {};
        sqlite3_bind_int(q,1,parameter);
        std::string result;
        if (sqlite3_step(q)==SQLITE_ROW) {
            const auto* value=sqlite3_column_text(q,0);
            if (value) result=reinterpret_cast<const char*>(value);
        }
        sqlite3_finalize(q);
        return result;
    }

    int submitAs(int studentId, const std::string& date, const std::string& path="sample.cpp") {
        int id=0;
        EXPECT_TRUE(workflow->submit(assignment,studentId,path,date,id)) << workflow->lastError();
        return id;
    }
};

TEST_F(WorkflowTests, RejectsUnauthorisedExtensionAndInvalidMark) {
    EXPECT_FALSE(workflow->grantIndividualExtension(assignment,student,hod,"2026-10-15","Reason"));
    int submission=submitAs(student,"2026-10-10");
    int mark=0;
    EXPECT_FALSE(workflow->recordMark(submission,lecturer,-1.0,"Feedback","Negative mark",mark));
    EXPECT_FALSE(workflow->recordMark(submission,lecturer,101.0,"Feedback","Above maximum",mark));
}


TEST_F(WorkflowTests, RejectsNonFiniteMarks) {
    int submission=submitAs(student,"2026-10-10");
    int mark=0;
    EXPECT_FALSE(workflow->recordMark(submission,lecturer,std::numeric_limits<double>::quiet_NaN(),"Feedback","NaN",mark));
    EXPECT_FALSE(workflow->recordMark(submission,lecturer,std::numeric_limits<double>::infinity(),"Feedback","Infinity",mark));
}

TEST_F(WorkflowTests, AcceptsZeroAndMaximumMarkBoundaries) {
    int zeroSubmission=submitAs(student,"2026-10-10","zero.cpp");
    int zeroMark=0;
    ASSERT_TRUE(workflow->recordMark(zeroSubmission,lecturer,0.0,"Zero mark","Valid lower boundary",zeroMark));
    EXPECT_DOUBLE_EQ(number("SELECT final_mark FROM marks WHERE mark_id=?",zeroMark),0.0);

    int maxSubmission=submitAs(student2,"2026-10-10","maximum.cpp");
    int maxMark=0;
    ASSERT_TRUE(workflow->recordMark(maxSubmission,lecturer,100.0,"Full marks","Valid upper boundary",maxMark));
    EXPECT_DOUBLE_EQ(number("SELECT final_mark FROM marks WHERE mark_id=?",maxMark),100.0);
}

TEST_F(WorkflowTests, LateResubmissionBecomesCurrentAndUsesItsOwnSubmissionDate) {
    int first=submitAs(student,"2026-10-10","first.cpp");
    int second=submitAs(student,"2026-10-11","late-resubmission.cpp");
    EXPECT_EQ(number("SELECT is_current FROM submissions WHERE submission_id=?",first),0.0);
    EXPECT_EQ(number("SELECT is_current FROM submissions WHERE submission_id=?",second),1.0);
    int mark=0;
    ASSERT_TRUE(workflow->recordMark(second,lecturer,80.0,"Late second version","Mark current version",mark));
    EXPECT_DOUBLE_EQ(number("SELECT final_mark FROM marks WHERE mark_id=?",mark),70.0);
}

TEST_F(WorkflowTests, RejectsInvalidAssignmentDetails) {
    int id=0;
    EXPECT_FALSE(workflow->createAssignment(course,lecturer,"","Description","2026-11-10",100.0,id));
    EXPECT_FALSE(workflow->createAssignment(course,lecturer,"Bad deadline","Description","not-a-date",100.0,id));
    EXPECT_FALSE(workflow->createAssignment(course,lecturer,"Bad maximum","Description","2026-11-10",0.0,id));
}

TEST_F(WorkflowTests, SubmissionOnDeadlineIsNotLate) {
    int submission=submitAs(student,"2026-10-10");
    int mark=0;
    ASSERT_TRUE(workflow->recordMark(submission,lecturer,80.0,"On time","Initial",mark)) << workflow->lastError();
    EXPECT_DOUBLE_EQ(number("SELECT penalty FROM marks WHERE mark_id=?",mark),0.0);
    EXPECT_DOUBLE_EQ(number("SELECT final_mark FROM marks WHERE mark_id=?",mark),80.0);
}

TEST_F(WorkflowTests, OneCalendarDayLateDeductsTenPercentOfMaximum) {
    int submission=submitAs(student,"2026-10-11");
    int mark=0;
    ASSERT_TRUE(workflow->recordMark(submission,lecturer,80.0,"One day late","Initial",mark)) << workflow->lastError();
    EXPECT_DOUBLE_EQ(number("SELECT penalty FROM marks WHERE mark_id=?",mark),10.0);
    EXPECT_DOUBLE_EQ(number("SELECT final_mark FROM marks WHERE mark_id=?",mark),70.0);
}

TEST_F(WorkflowTests, ThreeCalendarDaysLateDeductsThirtyPercentOfMaximum) {
    int submission=submitAs(student,"2026-10-13");
    int mark=0;
    ASSERT_TRUE(workflow->recordMark(submission,lecturer,80.0,"Three days late","Initial",mark)) << workflow->lastError();
    EXPECT_DOUBLE_EQ(number("SELECT penalty FROM marks WHERE mark_id=?",mark),30.0);
    EXPECT_DOUBLE_EQ(number("SELECT final_mark FROM marks WHERE mark_id=?",mark),50.0);
}

TEST_F(WorkflowTests, MoreThanThreeDaysLateIsAcceptedButReceivesZero) {
    int submission=submitAs(student,"2026-10-14");
    EXPECT_EQ(stringValue("SELECT status FROM submissions WHERE submission_id=?",submission),"LateZero");
    int mark=0;
    ASSERT_TRUE(workflow->recordMark(submission,lecturer,95.0,"Too late","Initial",mark)) << workflow->lastError();
    EXPECT_DOUBLE_EQ(number("SELECT penalty FROM marks WHERE mark_id=?",mark),100.0);
    EXPECT_DOUBLE_EQ(number("SELECT final_mark FROM marks WHERE mark_id=?",mark),0.0);
}

TEST_F(WorkflowTests, ResubmissionPreservesVersionsAndOnlyLatestIsCurrent) {
    int first=submitAs(student,"2026-10-09","version1.cpp");
    int second=submitAs(student,"2026-10-10","version2.cpp");
    EXPECT_EQ(number("SELECT COUNT(*) FROM submissions WHERE assignment_id=?",assignment),2.0);
    EXPECT_EQ(number("SELECT version_no FROM submissions WHERE submission_id=?",first),1.0);
    EXPECT_EQ(number("SELECT version_no FROM submissions WHERE submission_id=?",second),2.0);
    EXPECT_EQ(number("SELECT is_current FROM submissions WHERE submission_id=?",first),0.0);
    EXPECT_EQ(number("SELECT is_current FROM submissions WHERE submission_id=?",second),1.0);
    int mark=0;
    EXPECT_FALSE(workflow->recordMark(first,lecturer,60.0,"Old version","Should not mark",mark));
}

TEST_F(WorkflowTests, IndividualExtensionChangesOnlyThatStudentsEffectiveDeadline) {
    ASSERT_TRUE(workflow->grantIndividualExtension(assignment,student,lecturer,"2026-10-12","Documented illness")) << workflow->lastError();
    int extended=submitAs(student,"2026-10-11","extended.cpp");
    int unextended=submitAs(student2,"2026-10-11","normal.cpp");
    int mark1=0, mark2=0;
    ASSERT_TRUE(workflow->recordMark(extended,lecturer,80.0,"Extended deadline","Initial",mark1));
    ASSERT_TRUE(workflow->recordMark(unextended,lecturer,80.0,"Original deadline","Initial",mark2));
    EXPECT_DOUBLE_EQ(number("SELECT penalty FROM marks WHERE mark_id=?",mark1),0.0);
    EXPECT_DOUBLE_EQ(number("SELECT penalty FROM marks WHERE mark_id=?",mark2),10.0);
}

TEST_F(WorkflowTests, WholeClassExtensionChangesDeadlineForEveryEnrolledStudent) {
    ASSERT_TRUE(workflow->grantClassExtension(assignment,lecturer,"2026-10-12","Campus network outage")) << workflow->lastError();
    int one=submitAs(student,"2026-10-11","student1.cpp");
    int two=submitAs(student2,"2026-10-11","student2.cpp");
    int mark1=0, mark2=0;
    ASSERT_TRUE(workflow->recordMark(one,lecturer,75.0,"Class extension","Initial",mark1));
    ASSERT_TRUE(workflow->recordMark(two,lecturer,75.0,"Class extension","Initial",mark2));
    EXPECT_DOUBLE_EQ(number("SELECT penalty FROM marks WHERE mark_id=?",mark1),0.0);
    EXPECT_DOUBLE_EQ(number("SELECT penalty FROM marks WHERE mark_id=?",mark2),0.0);
}

TEST_F(WorkflowTests, RejectsInvalidExtensionAndUnenrolledStudent) {
    EXPECT_FALSE(workflow->grantIndividualExtension(assignment,student,lecturer,"2026-10-09","Earlier date"));
    EXPECT_FALSE(workflow->grantIndividualExtension(assignment,student,lecturer,"2026-10-12",""));
    int outsider=0;
    ASSERT_TRUE(users->createUser("outsider", "TestPassword1!", lms::domain::UserRole::Student, std::nullopt, outsider));
    EXPECT_FALSE(workflow->grantIndividualExtension(assignment,outsider,lecturer,"2026-10-12","Not enrolled"));
}

TEST_F(WorkflowTests, HoDCanRequestChangesButCannotReleaseUnapprovedMark) {
    int submission=submitAs(student,"2026-10-10");
    int mark=0;
    ASSERT_TRUE(workflow->recordMark(submission,lecturer,75.0,"Initial feedback","Initial mark",mark));
    ASSERT_TRUE(workflow->moderateMark(mark,hod,false,"Please review question 3.")) << workflow->lastError();
    EXPECT_EQ(stringValue("SELECT status FROM marks WHERE mark_id=?",mark),"Draft");
    EXPECT_FALSE(workflow->releaseMark(mark,hod));
    EXPECT_EQ(number("SELECT COUNT(*) FROM moderation_records WHERE mark_id=?",mark),1.0);
}

TEST_F(WorkflowTests, ApprovedMarkCanBeReleasedByHoD) {
    int submission=submitAs(student,"2026-10-10");
    int mark=0;
    ASSERT_TRUE(workflow->recordMark(submission,lecturer,75.0,"Good work","Initial mark",mark));
    EXPECT_FALSE(workflow->releaseMark(mark,hod));
    ASSERT_TRUE(workflow->moderateMark(mark,hod,true,"Approved")) << workflow->lastError();
    ASSERT_TRUE(workflow->releaseMark(mark,hod)) << workflow->lastError();
    EXPECT_EQ(stringValue("SELECT status FROM marks WHERE mark_id=?",mark),"Released");
}

TEST_F(WorkflowTests, MarkCorrectionCreatesAuditRecordWithOldAndNewValues) {
    int submission=submitAs(student,"2026-10-10");
    int mark=0;
    ASSERT_TRUE(workflow->recordMark(submission,lecturer,60.0,"First assessment","Initial mark",mark));
    ASSERT_TRUE(workflow->recordMark(submission,lecturer,70.0,"Corrected after review","Correction: calculation error",mark)) << workflow->lastError();
    EXPECT_EQ(number("SELECT COUNT(*) FROM mark_audit_log WHERE mark_id=?",mark),2.0);
    EXPECT_DOUBLE_EQ(number("SELECT old_value FROM mark_audit_log WHERE mark_id=? ORDER BY audit_id DESC LIMIT 1",mark),60.0);
    EXPECT_DOUBLE_EQ(number("SELECT new_value FROM mark_audit_log WHERE mark_id=? ORDER BY audit_id DESC LIMIT 1",mark),70.0);
    EXPECT_EQ(stringValue("SELECT reason FROM mark_audit_log WHERE mark_id=? ORDER BY audit_id DESC LIMIT 1",mark),"Correction: calculation error");
}

TEST_F(WorkflowTests, NonHoDCannotModerateOrReleaseMarks) {
    int submission=submitAs(student,"2026-10-10");
    int mark=0;
    ASSERT_TRUE(workflow->recordMark(submission,lecturer,70.0,"Feedback","Initial",mark));
    EXPECT_FALSE(workflow->moderateMark(mark,lecturer,true,"Not authorised"));
    EXPECT_FALSE(workflow->releaseMark(mark,lecturer));
}

TEST_F(WorkflowTests, StudentAssignmentListDisplaysWholeClassExtendedDeadline) {
    ASSERT_TRUE(workflow->grantClassExtension(assignment, lecturer,
        "2026-10-12", "Campus network outage")) << workflow->lastError();

    std::vector<lms::services::StudentAssignment> list;
    ASSERT_TRUE(workflow->listAssignmentsForStudent(student, list)) << workflow->lastError();
    ASSERT_EQ(list.size(), 1u);
    EXPECT_EQ(list[0].assignmentId, assignment);
    EXPECT_EQ(list[0].originalDeadline, "2026-10-10");
    EXPECT_EQ(list[0].effectiveDeadline, "2026-10-12");
    EXPECT_TRUE(list[0].deadlineExtended);
}

TEST_F(WorkflowTests, StudentAssignmentListShowsIndividualExtensionOnlyToThatStudent) {
    ASSERT_TRUE(workflow->grantIndividualExtension(assignment, student, lecturer,
        "2026-10-13", "Approved individual request")) << workflow->lastError();

    std::vector<lms::services::StudentAssignment> firstList, secondList;
    ASSERT_TRUE(workflow->listAssignmentsForStudent(student, firstList));
    ASSERT_TRUE(workflow->listAssignmentsForStudent(student2, secondList));
    ASSERT_EQ(firstList.size(), 1u);
    ASSERT_EQ(secondList.size(), 1u);
    EXPECT_EQ(firstList[0].effectiveDeadline, "2026-10-13");
    EXPECT_TRUE(firstList[0].deadlineExtended);
    EXPECT_EQ(secondList[0].effectiveDeadline, "2026-10-10");
    EXPECT_FALSE(secondList[0].deadlineExtended);
}

TEST_F(WorkflowTests, StudentAssignmentListRequiresActiveStudentRole) {
    std::vector<lms::services::StudentAssignment> list;
    EXPECT_FALSE(workflow->listAssignmentsForStudent(lecturer, list));
    EXPECT_TRUE(list.empty());
}
