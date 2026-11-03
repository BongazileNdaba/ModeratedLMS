
#ifndef MODERATED_LMS_ADMINISTRATOR_H
#define MODERATED_LMS_ADMINISTRATOR_H

#include "domain/User.h"
#include <utility>

namespace lms::domain {

class Administrator : public User {
public:
    // Administrators do not need an extra identifier in this model.
    Administrator(int userId,
                  std::string username,
                  std::optional<int> departmentId = std::nullopt,
                  bool active = true)
        : User(userId, std::move(username), departmentId, active) {
    }

    UserRole getRole() const override {
        return UserRole::Administrator;
    }
};

} // namespace lms::domain

#endif
