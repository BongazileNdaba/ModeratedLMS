
#include "services/AuthenticationService.h"

namespace lms::services {

AuthenticationService::AuthenticationService(
    lms::repositories::UserRepository& userRepository,
    const PasswordHasher& passwordHasher
)
    : userRepository_(userRepository),
      passwordHasher_(passwordHasher) {
}

std::optional<lms::repositories::StoredUser>
AuthenticationService::login(
    const std::string& username,
    const std::string& password
) {
    lastError_.clear();

    // Reject incomplete login requests before querying the database.
    if (username.empty() || password.empty()) {
        lastError_ = "Invalid username or password.";
        return std::nullopt;
    }

    // Retrieve the saved account, including its hash and salt.
    const auto user = userRepository_.findByUsername(username);

    if (!user.has_value()) {
        // Do not reveal whether the account exists.
        lastError_ = "Invalid username or password.";
        return std::nullopt;
    }

    // An inactive account must not be allowed to log in,
    // even when the supplied password is correct.
    if (!user->active) {
        lastError_ = "Invalid username or password.";
        return std::nullopt;
    }

    // Recalculate the password hash and compare it with the stored hash.
    const bool passwordMatches = passwordHasher_.verifyPassword(
        password,
        user->passwordHash,
        user->passwordSalt
    );

    if (!passwordMatches) {
        lastError_ = "Invalid username or password.";
        return std::nullopt;
    }

    // Authentication succeeded. Authorization is a separate responsibility:
    // this does not automatically grant permission to perform every action.
    return user;
}

const std::string& AuthenticationService::lastError() const {
    return lastError_;
}

} // namespace lms::services
