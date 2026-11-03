
#ifndef MODERATED_LMS_USER_H
#define MODERATED_LMS_USER_H

#include <optional>
#include <string>

namespace lms::domain {

// These are the four roles supported by our LMS.
// We use an enum instead of arbitrary role strings to reduce mistakes.
enum class UserRole {
    Administrator,
    Lecturer,
    HeadOfDepartment,
    Student
};

// Abstract base class: every LMS user shares these common properties.
// It is abstract because getRole() is a pure virtual function.
class User {
public:
    // Construct a user with their common identity information.
    User(int userId,
         std::string username,
         std::optional<int> departmentId,
         bool active = true)
        : userId_(userId),
          username_(std::move(username)),
          departmentId_(departmentId),
          active_(active) {
    }

    // A virtual destructor ensures derived objects are cleaned up safely
    // when deleted through a pointer to User.
    virtual ~User() = default;

    // Prevent accidental copying of user objects.
    User(const User&) = default;
    User& operator=(const User&) = default;

    // Each child class must identify its own role.
    virtual UserRole getRole() const = 0;

    // Read-only access to common user information.
    int getUserId() const {
        return userId_;
    }

    const std::string& getUsername() const {
        return username_;
    }

    std::optional<int> getDepartmentId() const {
        return departmentId_;
    }

    bool isActive() const {
        return active_;
    }

    // Allow the system to deactivate or reactivate an account.
    // Authorization checks will be performed in the service layer.
    void setActive(bool active) {
        active_ = active;
    }

private:
    // Private fields protect the object's internal state.
    int userId_;
    std::string username_;
    std::optional<int> departmentId_;
    bool active_;
};

} // namespace lms::domain

#endif
