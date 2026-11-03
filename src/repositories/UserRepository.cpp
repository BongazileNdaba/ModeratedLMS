
#include "repositories/UserRepository.h"

#include <sqlite3.h>

#include <utility>

namespace lms::repositories {

UserRepository::UserRepository(
    sqlite3* connection,
    const lms::services::PasswordHasher& passwordHasher
)
    : connection_(connection),
      passwordHasher_(passwordHasher) {
}

// Translate the C++ role into the exact text allowed by the database CHECK.
std::string UserRepository::roleToString(lms::domain::UserRole role) {
    using lms::domain::UserRole;

    switch (role) {
        case UserRole::Administrator:
            return "Administrator";
        case UserRole::Lecturer:
            return "Lecturer";
        case UserRole::HeadOfDepartment:
            return "HoD";
        case UserRole::Student:
            return "Student";
    }

    // An invalid enum value must never be saved as an account role.
    return {};
}

// Convert a database role into a valid C++ role.
bool UserRepository::stringToRole(
    const std::string& text,
    lms::domain::UserRole& role
) {
    using lms::domain::UserRole;

    if (text == "Administrator") {
        role = UserRole::Administrator;
    } else if (text == "Lecturer") {
        role = UserRole::Lecturer;
    } else if (text == "HoD") {
        role = UserRole::HeadOfDepartment;
    } else if (text == "Student") {
        role = UserRole::Student;
    } else {
        // Reject unknown role text instead of silently accepting it.
        return false;
    }

    return true;
}

bool UserRepository::createUser(
    const std::string& username,
    const std::string& password,
    lms::domain::UserRole role,
    std::optional<int> departmentId,
    int& newUserId
) {
    lastError_.clear();
    newUserId = 0;

    // Basic validation before using resources or writing to the database.
    if (connection_ == nullptr) {
        lastError_ = "Database connection is not available.";
        return false;
    }

    if (username.empty() || password.empty()) {
        lastError_ = "Username and password must not be empty.";
        return false;
    }

    const std::string roleText = roleToString(role);

    if (roleText.empty()) {
        lastError_ = "Invalid user role.";
        return false;
    }

    // Hash the password before saving anything.
    // The original password is never included in the SQL statement.
    lms::services::PasswordHashResult passwordResult;

    if (!passwordHasher_.hashPassword(password, passwordResult)) {
        lastError_ = "Could not securely hash the password.";
        return false;
    }

    // '?' placeholders are bound separately below.
    // This keeps user-provided values separate from executable SQL.
    const char* sql =
        "INSERT INTO users "
        "(username, password_hash, password_salt, role, "
        "department_id, is_active) "
        "VALUES (?, ?, ?, ?, ?, 1);";

    sqlite3_stmt* statement = nullptr;

    if (sqlite3_prepare_v2(
            connection_, sql, -1, &statement, nullptr) != SQLITE_OK) {
        lastError_ = sqlite3_errmsg(connection_);
        return false;
    }

    // Bind each value to its placeholder.
    // SQLITE_TRANSIENT tells SQLite to make its own safe copy of the text.
    bool bindingsSucceeded = true;

    bindingsSucceeded &=
        sqlite3_bind_text(statement, 1, username.c_str(),
                          -1, SQLITE_TRANSIENT) == SQLITE_OK;

    bindingsSucceeded &=
        sqlite3_bind_text(statement, 2, passwordResult.hash.c_str(),
                          -1, SQLITE_TRANSIENT) == SQLITE_OK;

    bindingsSucceeded &=
        sqlite3_bind_text(statement, 3, passwordResult.salt.c_str(),
                          -1, SQLITE_TRANSIENT) == SQLITE_OK;

    bindingsSucceeded &=
        sqlite3_bind_text(statement, 4, roleText.c_str(),
                          -1, SQLITE_TRANSIENT) == SQLITE_OK;

    // Bind SQL NULL when this user has no department.
    if (departmentId.has_value()) {
        bindingsSucceeded &=
            sqlite3_bind_int(statement, 5, *departmentId) == SQLITE_OK;
    } else {
        bindingsSucceeded &=
            sqlite3_bind_null(statement, 5) == SQLITE_OK;
    }

    if (!bindingsSucceeded) {
        lastError_ = sqlite3_errmsg(connection_);
        sqlite3_finalize(statement);
        return false;
    }

    // Execute the INSERT and check whether SQLite accepted the account.
    const int result = sqlite3_step(statement);

    if (result != SQLITE_DONE) {
        lastError_ = sqlite3_errmsg(connection_);
        sqlite3_finalize(statement);
        return false;
    }

    // Capture the generated primary key before finalizing the statement.
    newUserId = static_cast<int>(sqlite3_last_insert_rowid(connection_));

    // Prepared statements must be finalized to release SQLite resources.
    const int finalizeResult = sqlite3_finalize(statement);

    if (finalizeResult != SQLITE_OK) {
        lastError_ = sqlite3_errmsg(connection_);
        newUserId = 0;
        return false;
    }

    return true;
}

std::optional<StoredUser> UserRepository::findByUsername(
    const std::string& username
) const {
    lastError_.clear();

    if (connection_ == nullptr) {
        lastError_ = "Database connection is not available.";
        return std::nullopt;
    }

    const char* sql =
        "SELECT user_id, username, password_hash, password_salt, "
        "role, department_id, is_active "
        "FROM users WHERE username = ? LIMIT 1;";

    sqlite3_stmt* statement = nullptr;

    if (sqlite3_prepare_v2(
            connection_, sql, -1, &statement, nullptr) != SQLITE_OK) {
        lastError_ = sqlite3_errmsg(connection_);
        return std::nullopt;
    }

    if (sqlite3_bind_text(
            statement, 1, username.c_str(),
            -1, SQLITE_TRANSIENT) != SQLITE_OK) {
        lastError_ = sqlite3_errmsg(connection_);
        sqlite3_finalize(statement);
        return std::nullopt;
    }

    // SQLITE_ROW means a matching user was found.
    const int result = sqlite3_step(statement);

    if (result == SQLITE_DONE) {
        // No matching username exists; this is not a database error.
        sqlite3_finalize(statement);
        return std::nullopt;
    }

    if (result != SQLITE_ROW) {
        lastError_ = sqlite3_errmsg(connection_);
        sqlite3_finalize(statement);
        return std::nullopt;
    }

    // Read the account fields from the result row.
    StoredUser user;
    user.userId = sqlite3_column_int(statement, 0);

    // SQLite owns the returned text pointers, so copy them into std::string.
    const auto readText = [statement](int column) -> std::string {
        const unsigned char* value = sqlite3_column_text(statement, column);
        return value != nullptr
            ? reinterpret_cast<const char*>(value)
            : "";
    };

    user.username = readText(1);
    user.passwordHash = readText(2);
    user.passwordSalt = readText(3);

    if (!stringToRole(readText(4), user.role)) {
        lastError_ = "Database contains an unknown user role.";
        sqlite3_finalize(statement);
        return std::nullopt;
    }

    if (sqlite3_column_type(statement, 5) != SQLITE_NULL) {
        user.departmentId = sqlite3_column_int(statement, 5);
    }

    user.active = sqlite3_column_int(statement, 6) != 0;

    if (sqlite3_finalize(statement) != SQLITE_OK) {
        lastError_ = sqlite3_errmsg(connection_);
        return std::nullopt;
    }

    return user;
}

const std::string& UserRepository::lastError() const {
    return lastError_;
}

bool UserRepository::setUserActive(int userId, bool active) {
    lastError_.clear();

    // A database ID must be positive to identify a valid account.
    if (userId <= 0) {
        lastError_ = "Invalid user ID.";
        return false;
    }

    // Use a prepared statement so the values are bound safely.
    const char* sql =
        "UPDATE users SET is_active = ? WHERE user_id = ?;";

    sqlite3_stmt* statement = nullptr;

    if (sqlite3_prepare_v2(
            connection_, sql, -1, &statement, nullptr) != SQLITE_OK) {
        lastError_ = sqlite3_errmsg(connection_);
        return false;
    }

    // SQLite stores the active status as an integer: 1 or 0.
    bool bindingsSucceeded = true;

    if (sqlite3_bind_int(statement, 1, active ? 1 : 0) != SQLITE_OK) {
        bindingsSucceeded = false;
    }

    if (sqlite3_bind_int(statement, 2, userId) != SQLITE_OK) {
        bindingsSucceeded = false;
    }

    if (!bindingsSucceeded) {
        lastError_ = sqlite3_errmsg(connection_);
        sqlite3_finalize(statement);
        return false;
    }

    // Execute the UPDATE statement.
    const int result = sqlite3_step(statement);

    if (result != SQLITE_DONE) {
        lastError_ = sqlite3_errmsg(connection_);
        sqlite3_finalize(statement);
        return false;
    }

    // Check that SQLite actually found an account with this ID.
    const int affectedRows = sqlite3_changes(connection_);

    sqlite3_finalize(statement);

    if (affectedRows == 0) {
        lastError_ = "No account found with that user ID.";
        return false;
    }

    return true;
}


} // namespace lms::repositories
