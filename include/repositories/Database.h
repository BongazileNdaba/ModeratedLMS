//the following class opens the database and create tables, 
//gives repositories classes access to SQite when we build those classes and 
// lets us explain why an operation failed

#ifndef MODERATED_LMS_DATABASE_H
#define MODERATED_LMS_DATABASE_H

#include <sqlite3.h>
#include <string>

namespace lms::repositories {

// Responsible for opening, initializing, and closing the SQLite database.
class Database {
public:
    // The database file will be created if it does not already exist.
    explicit Database(const std::string& databasePath);

    // Close the database connection when the object is destroyed.
    ~Database();

    // Prevent accidental copying of the database connection.
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;

    // Open the connection and create the tables if necessary.
    bool initialize();

    // Allow other repository classes to use the connection later.
    sqlite3* connection() const;

    // Return the most recent database error.
    const std::string& lastError() const;

private:
    std::string databasePath_;
    std::string lastError_;
    sqlite3* connection_ = nullptr;

    // Execute a SQL statement and remember any error.
    bool execute(const char* sql);
};

} // namespace lms::repositories

#endif