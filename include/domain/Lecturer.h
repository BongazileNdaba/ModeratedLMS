
#ifndef MODERATED_LMS_LECTURER_H
#define MODERATED_LMS_LECTURER_H

#include "domain/User.h"
#include <string>
#include <utility>

namespace lms::domain {

class Lecturer : public User {
public:
    // A lecturer also has a staff number.
    Lecturer(int userId,
             std::string username,
             std::string staffNumber,
             std::optional<int> departmentId = std::nullopt,
             bool active = true)
        : User(userId, std::move(username), departmentId, active),
          staffNumber_(std::move(staffNumber)) {
    }

    UserRole getRole() const override {
        return UserRole::Lecturer;
    }

    const std::string& getStaffNumber() const {
        return staffNumber_;
    }

private:
    std::string staffNumber_;
};

} // namespace lms::domain

#endif
