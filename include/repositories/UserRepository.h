
#ifndef MODERATED_LMS_USER_REPOSITORY_H
#define MODERATED_LMS_USER_REPOSITORY_H

#include "domain/User.h"
#include "services/PasswordHasher.h"

#include <optional>
#include <string>
#include <sqlite3.h>

namespace lms::repositories {

// Represents the account information needed for persistence and login.
// Passwords themselves are deliberately not included in this structure.
struct StoredUser {
    int userId = 0;
    std::string username;
    std::string passwordHash;
    std::string passwordSalt;
    lms::domain::UserRole role;
    std::optional<int> departmentId;
    bool active = true;
};

class UserRepository {
public:
    // Use the existing database connection and password-hashing service.
    UserRepository(
        sqlite3* connection,
        const lms::services::PasswordHasher& passwordHasher
    );

    // Hash the password and insert a new account into SQLite.
    // On success, newUserId receives the generated database ID.
    bool createUser(
        const std::string& username,
        const std::string& password,
        lms::domain::UserRole role,
        std::optional<int> departmentId,
        int& newUserId
    );

    // Find an account by username.
    // Returns no value if no matching account exists.
    // Check lastError() to distinguish a database error from no match.
    std::optional<StoredUser> findByUsername(
        const std::string& username
    ) const;

    // Enable or disable an existing account without deleting its records.
    // Returns true if the account status was updated successfully.
    bool setUserActive(int userId, bool active);

    // Expose the most recent error for troubleshooting and reporting.
    const std::string& lastError() const;

private:
    sqlite3* connection_;
    const lms::services::PasswordHasher& passwordHasher_;
    mutable std::string lastError_;

    // Convert the C++ role enumeration to the exact role text in SQLite.
    static std::string roleToString(lms::domain::UserRole role);

    // Convert a role read from SQLite back to the C++ enumeration.
    static bool stringToRole(
        const std::string& text,
        lms::domain::UserRole& role
    );
};

} // namespace lms::repositories

#endif
