
#include <gtest/gtest.h>

#include "services/PasswordHasher.h"

#include <string>

// These tests check the password hashing and verification functions.
class PasswordHasherTest : public ::testing::Test
{
protected:
    // Create one PasswordHasher instance for each test.
    lms::services::PasswordHasher passwordHasher;
};

// Test that a password can be hashed successfully.
TEST_F(PasswordHasherTest, HashesPasswordSuccessfully)
{
    lms::services::PasswordHashResult result;

    const bool success =
        passwordHasher.hashPassword("MySecurePassword123!", result);

    EXPECT_TRUE(success);
    EXPECT_FALSE(result.hash.empty());
    EXPECT_FALSE(result.salt.empty());
}

// Test that the generated hash and salt have the expected lengths.
// The hash is 32 bytes (64 hexadecimal characters).
// The salt is 16 bytes (32 hexadecimal characters).
TEST_F(PasswordHasherTest, GeneratesCorrectHashAndSaltLengths)
{
    lms::services::PasswordHashResult result;

    ASSERT_TRUE(
        passwordHasher.hashPassword("MySecurePassword123!", result));

    EXPECT_EQ(result.hash.size(), 64);
    EXPECT_EQ(result.salt.size(), 32);
}

// Test that the original password matches its generated hash.
TEST_F(PasswordHasherTest, VerifiesCorrectPassword)
{
    lms::services::PasswordHashResult result;

    ASSERT_TRUE(
        passwordHasher.hashPassword("MySecurePassword123!", result));

    EXPECT_TRUE(
        passwordHasher.verifyPassword(
            "MySecurePassword123!",
            result.hash,
            result.salt));
}

// Test that an incorrect password is rejected.
TEST_F(PasswordHasherTest, RejectsIncorrectPassword)
{
    lms::services::PasswordHashResult result;

    ASSERT_TRUE(
        passwordHasher.hashPassword("MySecurePassword123!", result));

    EXPECT_FALSE(
        passwordHasher.verifyPassword(
            "WrongPassword123!",
            result.hash,
            result.salt));
}

// Test that hashing the same password twice generates different salts.
TEST_F(PasswordHasherTest, GeneratesDifferentSaltsForSamePassword)
{
    lms::services::PasswordHashResult first;
    lms::services::PasswordHashResult second;

    ASSERT_TRUE(
        passwordHasher.hashPassword("MySecurePassword123!", first));

    ASSERT_TRUE(
        passwordHasher.hashPassword("MySecurePassword123!", second));

    EXPECT_NE(first.salt, second.salt);
    EXPECT_NE(first.hash, second.hash);
}

// Test that an empty password cannot be hashed.
TEST_F(PasswordHasherTest, RejectsEmptyPasswordWhenHashing)
{
    lms::services::PasswordHashResult result;

    EXPECT_FALSE(passwordHasher.hashPassword("", result));
}

// Test that an empty password cannot be verified.
TEST_F(PasswordHasherTest, RejectsEmptyPasswordWhenVerifying)
{
    EXPECT_FALSE(
        passwordHasher.verifyPassword("", "somehash", "somesalt"));
}

// Test that incorrectly sized stored values are rejected.
TEST_F(PasswordHasherTest, RejectsIncorrectHashOrSaltLengths)
{
    EXPECT_FALSE(
        passwordHasher.verifyPassword(
            "MySecurePassword123!",
            "short_hash",
            std::string(32, 'a')));

    EXPECT_FALSE(
        passwordHasher.verifyPassword(
            "MySecurePassword123!",
            std::string(64, 'a'),
            "short_salt"));
}

// Test that invalid hexadecimal characters are rejected.
TEST_F(PasswordHasherTest, RejectsInvalidHexadecimalValues)
{
    EXPECT_FALSE(
        passwordHasher.verifyPassword(
            "MySecurePassword123!",
            std::string(64, 'z'),
            std::string(32, 'a')));
}
