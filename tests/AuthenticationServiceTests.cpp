
#include <gtest/gtest.h>

#include "repositories/Database.h"
#include "repositories/UserRepository.h"
#include "services/AuthenticationService.h"
#include "services/PasswordHasher.h"
#include "domain/User.h"

#include <cstdio>
#include <string>

// These tests use a separate SQLite database so they do not
// modify the actual Moderated LMS database.
class AuthenticationServiceTest : public ::testing::Test
{
protected:
    std::string databasePath = "authentication_test.db";

    lms::services::PasswordHasher passwordHasher;
    lms::repositories::Database database{databasePath};

    std::unique_ptr<lms::repositories::UserRepository> userRepository;
    std::unique_ptr<lms::services::AuthenticationService> authService;

    void SetUp() override
    {
        // Remove an old test database, if one exists.
        std::remove(databasePath.c_str());

        // Initialize SQLite and create the required tables.
        ASSERT_TRUE(database.initialize())
            << database.lastError();

        // Connect the repository and authentication service.
        userRepository =
            std::make_unique<lms::repositories::UserRepository>(
                database.connection(), passwordHasher);

        authService =
            std::make_unique<lms::services::AuthenticationService>(
                *userRepository, passwordHasher);
    }

    void TearDown() override
    {
        // Destroy services before closing the database connection.
        authService.reset();
        userRepository.reset();
    }

    int createTestUser(
        const std::string &username,
        const std::string &password)
    {
        int userId = 0;

        // Create a student account using the real password hasher.
        const bool created = userRepository->createUser(
            username,
            password,
            lms::domain::UserRole::Student,
            std::nullopt,
            userId);

        EXPECT_TRUE(created) << userRepository->lastError();
        return userId;
    }
};

// A correct username and password should allow login.
TEST_F(AuthenticationServiceTest, SuccessfulLogin)
{
    createTestUser("student1", "CorrectPassword123!");

    const auto result =
        authService->login("student1", "CorrectPassword123!");

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->username, "student1");
}

// An incorrect password must be rejected.
TEST_F(AuthenticationServiceTest, RejectsIncorrectPassword)
{
    createTestUser("student2", "CorrectPassword123!");

    const auto result =
        authService->login("student2", "WrongPassword123!");

    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(
        authService->lastError(),
        "Invalid username or password.");
}

// An account that does not exist must be rejected.
TEST_F(AuthenticationServiceTest, RejectsUnknownUsername)
{
    const auto result =
        authService->login("unknown_student", "SomePassword123!");

    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(
        authService->lastError(),
        "Invalid username or password.");
}

// An inactive account must not be allowed to log in.
TEST_F(AuthenticationServiceTest, RejectsInactiveAccount)
{
    const int userId =
        createTestUser("inactive_student", "CorrectPassword123!");

    ASSERT_TRUE(userRepository->setUserActive(userId, false));

    const auto result =
        authService->login("inactive_student", "CorrectPassword123!");

    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(
        authService->lastError(),
        "Invalid username or password.");
}

// An empty username must be rejected.
TEST_F(AuthenticationServiceTest, RejectsEmptyUsername)
{
    const auto result =
        authService->login("", "SomePassword123!");

    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(
        authService->lastError(),
        "Invalid username or password.");
}

// An empty password must be rejected.
TEST_F(AuthenticationServiceTest, RejectsEmptyPassword)
{
    const auto result =
        authService->login("student1", "");

    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(
        authService->lastError(),
        "Invalid username or password.");
}

// All authentication failures should use the same generic error.
TEST_F(AuthenticationServiceTest, UsesGenericErrorForFailedLogins)
{
    const auto unknownUser =
        authService->login("missing_user", "Password123!");

    const std::string unknownUserError = authService->lastError();

    createTestUser("known_student", "CorrectPassword123!");

    const auto wrongPassword =
        authService->login("known_student", "WrongPassword123!");

    const std::string wrongPasswordError = authService->lastError();

    EXPECT_FALSE(unknownUser.has_value());
    EXPECT_FALSE(wrongPassword.has_value());
    EXPECT_EQ(unknownUserError, wrongPasswordError);
}
