
#ifndef MODERATED_LMS_STUDENT_H
#define MODERATED_LMS_STUDENT_H

#include "domain/User.h"
#include <string>
#include <utility>

namespace lms::domain {

class Student : public User {
public:
    // A student has the common User fields plus a student number.
    Student(int userId,
            std::string username,
            std::string studentNumber,
            std::optional<int> departmentId = std::nullopt,
            bool active = true)
        : User(userId, std::move(username), departmentId, active),
          studentNumber_(std::move(studentNumber)) {
    }

    // Tell the rest of the system that this object is a Student.
    UserRole getRole() const override {
        return UserRole::Student;
    }

    const std::string& getStudentNumber() const {
        return studentNumber_;
    }

private:
    std::string studentNumber_;
};

} // namespace lms::domain

#endif
