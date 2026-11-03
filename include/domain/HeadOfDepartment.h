
#ifndef MODERATED_LMS_HEAD_OF_DEPARTMENT_H
#define MODERATED_LMS_HEAD_OF_DEPARTMENT_H

#include "domain/User.h"
#include <utility>

namespace lms::domain {

class HeadOfDepartment : public User {
public:
    // The HoD inherits common identity information from User.
    HeadOfDepartment(int userId,
                     std::string username,
                     std::optional<int> departmentId = std::nullopt,
                     bool active = true)
        : User(userId, std::move(username), departmentId, active) {
    }

    UserRole getRole() const override {
        return UserRole::HeadOfDepartment;
    }
};

} // namespace lms::domain

#endif
