#include "repositories/Database.h"

#include <iostream>

namespace lms::repositories {

Database::Database(const std::string& databasePath)
    : databasePath_(databasePath) {
    // Save the file path. The connection is opened by initialize().
}

Database::~Database() {
    // Release SQLite resources when this object is no longer needed.
    if (connection_ != nullptr) {
        sqlite3_close(connection_);
        connection_ = nullptr;
    }
}

bool Database::initialize() {
    // Open the existing database, or create it if it does not exist.
    const int result = sqlite3_open(
        databasePath_.c_str(),
        &connection_
    );

    if (result != SQLITE_OK) {
        lastError_ = connection_ != nullptr
            ? sqlite3_errmsg(connection_)
            : "Could not allocate a SQLite connection.";

        std::cerr << "Database opening failed: "
                  << lastError_ << '\n';
        return false;
    }

    // Enable foreign-key enforcement for this connection.
    if (!execute("PRAGMA foreign_keys = ON;")) {
        return false;
    }

    // Create the tables only if they do not already exist.
    // Existing data is preserved when the program is restarted.
    const char* schema = R"SQL(

        CREATE TABLE IF NOT EXISTS departments (
            department_id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL UNIQUE
        );

        CREATE TABLE IF NOT EXISTS users (
            user_id INTEGER PRIMARY KEY AUTOINCREMENT,
            username TEXT NOT NULL UNIQUE,
            password_hash TEXT NOT NULL,
            password_salt TEXT NOT NULL,
            role TEXT NOT NULL CHECK (
                role IN ('Administrator', 'Lecturer', 'HoD', 'Student')
            ),
            department_id INTEGER,
            is_active INTEGER NOT NULL DEFAULT 1
                CHECK (is_active IN (0, 1)),
            FOREIGN KEY (department_id)
                REFERENCES departments(department_id)
        );

        CREATE TABLE IF NOT EXISTS courses (
            course_id INTEGER PRIMARY KEY AUTOINCREMENT,
            department_id INTEGER NOT NULL,
            course_code TEXT NOT NULL UNIQUE,
            title TEXT NOT NULL,
            FOREIGN KEY (department_id)
                REFERENCES departments(department_id)
        );

        CREATE TABLE IF NOT EXISTS enrollments (
            enrollment_id INTEGER PRIMARY KEY AUTOINCREMENT,
            student_id INTEGER NOT NULL,
            course_id INTEGER NOT NULL,
            enrolled_at TEXT NOT NULL,
            UNIQUE (student_id, course_id),
            FOREIGN KEY (student_id) REFERENCES users(user_id),
            FOREIGN KEY (course_id) REFERENCES courses(course_id)
        );

        CREATE TABLE IF NOT EXISTS assignments (
            assignment_id INTEGER PRIMARY KEY AUTOINCREMENT,
            course_id INTEGER NOT NULL,
            lecturer_id INTEGER NOT NULL,
            title TEXT NOT NULL,
            description TEXT NOT NULL DEFAULT '',
            deadline TEXT NOT NULL,
            max_mark REAL NOT NULL CHECK (max_mark > 0),
            created_at TEXT NOT NULL,
            FOREIGN KEY (course_id) REFERENCES courses(course_id),
            FOREIGN KEY (lecturer_id) REFERENCES users(user_id)
        );

        CREATE TABLE IF NOT EXISTS individual_extensions (
            extension_id INTEGER PRIMARY KEY AUTOINCREMENT,
            assignment_id INTEGER NOT NULL,
            student_id INTEGER NOT NULL,
            revised_deadline TEXT NOT NULL,
            approved_by_lecturer_id INTEGER NOT NULL,
            approved_at TEXT NOT NULL,
            reason TEXT NOT NULL,
            UNIQUE (assignment_id, student_id),
            FOREIGN KEY (assignment_id)
                REFERENCES assignments(assignment_id),
            FOREIGN KEY (student_id) REFERENCES users(user_id),
            FOREIGN KEY (approved_by_lecturer_id)
                REFERENCES users(user_id)
        );

        CREATE TABLE IF NOT EXISTS submissions (
            submission_id INTEGER PRIMARY KEY AUTOINCREMENT,
            assignment_id INTEGER NOT NULL,
            student_id INTEGER NOT NULL,
            version_no INTEGER NOT NULL,
            file_path TEXT NOT NULL,
            submitted_at TEXT NOT NULL,
            status TEXT NOT NULL,
            is_current INTEGER NOT NULL CHECK (is_current IN (0, 1)),
            UNIQUE (assignment_id, student_id, version_no),
            FOREIGN KEY (assignment_id)
                REFERENCES assignments(assignment_id),
            FOREIGN KEY (student_id) REFERENCES users(user_id)
        );

        CREATE TABLE IF NOT EXISTS marks (
            mark_id INTEGER PRIMARY KEY AUTOINCREMENT,
            submission_id INTEGER NOT NULL UNIQUE,
            value REAL CHECK (value IS NULL OR value >= 0),
            feedback TEXT,
            status TEXT NOT NULL CHECK (
                status IN (
                    'Draft',
                    'Marked',
                    'AwaitingModeration',
                    'Approved',
                    'Released'
                )
            ),
            FOREIGN KEY (submission_id)
                REFERENCES submissions(submission_id)
        );

        CREATE TABLE IF NOT EXISTS moderation_records (
            moderation_id INTEGER PRIMARY KEY AUTOINCREMENT,
            mark_id INTEGER NOT NULL,
            hod_user_id INTEGER NOT NULL,
            decision TEXT NOT NULL CHECK (
                decision IN ('Approved', 'ChangesRequested')
            ),
            decision_at TEXT NOT NULL,
            comments TEXT,
            FOREIGN KEY (mark_id) REFERENCES marks(mark_id),
            FOREIGN KEY (hod_user_id) REFERENCES users(user_id)
        );

        CREATE TABLE IF NOT EXISTS mark_audit_log (
            audit_id INTEGER PRIMARY KEY AUTOINCREMENT,
            mark_id INTEGER NOT NULL,
            changed_by_user_id INTEGER NOT NULL,
            changed_at TEXT NOT NULL,
            old_value REAL,
            new_value REAL,
            reason TEXT NOT NULL,
            FOREIGN KEY (mark_id) REFERENCES marks(mark_id),
            FOREIGN KEY (changed_by_user_id) REFERENCES users(user_id)
        );

        -- A student can have only one current submission per assignment.
        CREATE UNIQUE INDEX IF NOT EXISTS
            one_current_submission_per_student_assignment
        ON submissions (assignment_id, student_id)
        WHERE is_current = 1;

    )SQL";

    if (!execute(schema)) {
        return false;
    }

    std::cout << "Database initialized successfully.\n";
    return true;
}

bool Database::execute(const char* sql) {
    char* errorMessage = nullptr;

    // Execute SQL. SQLite supplies an error message if execution fails.
    const int result = sqlite3_exec(
        connection_,
        sql,
        nullptr,
        nullptr,
        &errorMessage
    );

    if (result != SQLITE_OK) {
        lastError_ = errorMessage != nullptr
            ? errorMessage
            : sqlite3_errmsg(connection_);

        std::cerr << "Database error: " << lastError_ << '\n';

        // SQLite allocates this error message, so we must release it.
        sqlite3_free(errorMessage);
        return false;
    }

    return true;
}

sqlite3* Database::connection() const {
    return connection_;
}

const std::string& Database::lastError() const {
    return lastError_;
}

} // namespace lms::repositories