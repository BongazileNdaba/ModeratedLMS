#ifndef MODERATED_LMS_WORKFLOW_SERVICE_H
#define MODERATED_LMS_WORKFLOW_SERVICE_H

#include <sqlite3.h>
#include <string>
#include <vector>

namespace lms::services {

// One row in the student-facing assignment list. The effective deadline is
// the deadline the student must follow after applicable extensions.
struct StudentAssignment {
    int assignmentId{};
    std::string title;
    std::string description;
    std::string originalDeadline;
    std::string effectiveDeadline;
    double maximumMark{};
    bool deadlineExtended{};
};

// Implements the LMS business rules for assignment extensions, submissions,
// marking, moderation and mark release. Database access stays behind this service.
class WorkflowService {
public:
    explicit WorkflowService(sqlite3* connection);

    bool createAssignment(int courseId, int lecturerId, const std::string& title,
                          const std::string& description, const std::string& deadline,
                          double maximumMark, int& assignmentId);

    bool grantIndividualExtension(int assignmentId, int studentId,
                                  int lecturerId, const std::string& revisedDeadline,
                                  const std::string& reason);

    bool grantClassExtension(int assignmentId, int lecturerId,
                             const std::string& revisedDeadline,
                             const std::string& reason);

    // Returns assignments for the student's enrolled courses, displaying each
    // student's own effective deadline (class-wide or individual extension).
    bool listAssignmentsForStudent(int studentId,
                                    std::vector<StudentAssignment>& assignments);

    bool submit(int assignmentId, int studentId, const std::string& filePath,
                const std::string& submittedAt, int& submissionId);

    // rawMark is the lecturer's mark before late penalties are applied.
    bool recordMark(int submissionId, int lecturerId, double rawMark,
                    const std::string& feedback, const std::string& reason,
                    int& markId);

    bool moderateMark(int markId, int hodId, bool approve,
                      const std::string& comments);

    bool releaseMark(int markId, int hodId);

    const std::string& lastError() const;

private:
    sqlite3* db_;
    std::string lastError_;

    bool execute(const char* sql);
    bool isEnrolled(int studentId, int courseId);
    bool hasRole(int userId, const std::string& role);
    bool assignmentDetails(int assignmentId, int& courseId, double& maximumMark,
                           std::string& deadline, int& lecturerId);
    std::string effectiveDeadline(int assignmentId, int studentId,
                                  const std::string& baseDeadline);
    static int calendarDaysLate(const std::string& deadline,
                                const std::string& submittedAt);
};

} // namespace lms::services
#endif
