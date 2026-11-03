
#ifndef MODERATED_LMS_PASSWORD_HASHER_H
#define MODERATED_LMS_PASSWORD_HASHER_H

#include <string>

namespace lms::services {

// Holds the two values that must be saved in the users table.
// Both values are hexadecimal strings, so they can be stored as TEXT.
struct PasswordHashResult {
    std::string hash;
    std::string salt;
};

// Provides password hashing and verification using libsodium.
class PasswordHasher {
public:
    // Prepare libsodium before using its cryptographic functions.
    PasswordHasher();

    // Generate a fresh random salt and derive a password hash.
    // Returns false if hashing fails.
    bool hashPassword(
        const std::string& password,
        PasswordHashResult& result
    ) const;

    // Verify a password against the previously stored hash and salt.
    // Returns false if the password does not match or the data is invalid.
    bool verifyPassword(
        const std::string& password,
        const std::string& storedHash,
        const std::string& storedSalt
    ) const;

private:
    // Convert raw cryptographic bytes into hexadecimal text for SQLite.
    static std::string toHex(const unsigned char* bytes, std::size_t length);

    // Convert hexadecimal text back into bytes.
    // Invalid hexadecimal input is rejected.
    static bool fromHex(
        const std::string& hex,
        unsigned char* output,
        std::size_t outputLength
    );
};

} // namespace lms::services

#endif
