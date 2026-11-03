
#ifndef MODERATED_LMS_AUTHENTICATION_SERVICE_H
#define MODERATED_LMS_AUTHENTICATION_SERVICE_H

#include "repositories/UserRepository.h"

#include <optional>
#include <string>

namespace lms::services {

// Handles login rules while leaving SQL to UserRepository
// and password verification to PasswordHasher.
class AuthenticationService {
public:
    explicit AuthenticationService(
        lms::repositories::UserRepository& userRepository,
        const PasswordHasher& passwordHasher
    );

    // Return account information only when authentication succeeds.
    // A missing result means the login was rejected.
    std::optional<lms::repositories::StoredUser> login(
        const std::string& username,
        const std::string& password
    );

    // A generic message avoids revealing whether a username exists.
    const std::string& lastError() const;

private:
    lms::repositories::UserRepository& userRepository_;
    const PasswordHasher& passwordHasher_;
    std::string lastError_;
};

} // namespace lms::services

#endif
