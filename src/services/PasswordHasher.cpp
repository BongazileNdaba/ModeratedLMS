
#include "services/PasswordHasher.h"

#include <sodium.h>

#include <stdexcept>
#include <utility>
#include <vector>

namespace lms::services
{

    // Use a fixed 32-byte derived hash. The salt is generated separately.
    namespace
    {
        constexpr std::size_t HASH_BYTES = 32;
        constexpr std::size_t SALT_BYTES = crypto_pwhash_SALTBYTES;

        // Use libsodium's interactive password-hashing settings as our initial
        // security/performance choice. These settings should be tested on the
        // school's actual computer before deployment.
        constexpr unsigned long long OPS_LIMIT =
            crypto_pwhash_OPSLIMIT_INTERACTIVE;

        constexpr std::size_t MEM_LIMIT =
            crypto_pwhash_MEMLIMIT_INTERACTIVE;
    }

    // Initialize libsodium once when a PasswordHasher object is constructed.
    PasswordHasher::PasswordHasher()
    {
        if (sodium_init() < 0)
        {
            // Cryptographic functions must not be used if initialization fails.
            throw std::runtime_error("Could not initialize libsodium.");
        }
    }

    bool PasswordHasher::hashPassword(
        const std::string &password,
        PasswordHashResult &result) const
    {
        // An empty password should not be accepted as a valid account password.
        if (password.empty())
        {
            return false;
        }

        // Allocate the salt as raw bytes. It is random, not derived from the
        // username or password, so identical passwords can have different salts.
        unsigned char salt[SALT_BYTES];
        randombytes_buf(salt, sizeof(salt));

        // Store the derived hash in a fixed-size byte buffer.
        unsigned char hash[HASH_BYTES];

        // Derive a password hash using the password, random salt and chosen cost.
        // This function is intentionally computationally expensive to make
        // password guessing more difficult.
        const int status = crypto_pwhash(
            hash,
            sizeof(hash),
            password.data(),
            static_cast<unsigned long long>(password.size()),
            salt,
            OPS_LIMIT,
            MEM_LIMIT,
            crypto_pwhash_ALG_ARGON2ID13);

        if (status != 0)
        {
            // The operation can fail, for example if sufficient memory is
            // unavailable. Never save an incomplete password hash.
            sodium_memzero(hash, sizeof(hash));
            sodium_memzero(salt, sizeof(salt));
            return false;
        }

        // Convert raw bytes to text before returning them for SQLite storage.
        result.hash = toHex(hash, sizeof(hash));
        result.salt = toHex(salt, sizeof(salt));

        // Clear sensitive intermediate buffers when they are no longer needed.
        sodium_memzero(hash, sizeof(hash));
        sodium_memzero(salt, sizeof(salt));

        return true;
    }

    bool PasswordHasher::verifyPassword(
        const std::string &password,
        const std::string &storedHash,
        const std::string &storedSalt) const
    {
        // Reject empty passwords and incorrectly sized stored values.
        if (password.empty() ||
            storedHash.size() != HASH_BYTES * 2 ||
            storedSalt.size() != SALT_BYTES * 2)
        {
            return false;
        }

        // Convert the stored hexadecimal salt and hash back to raw bytes.
        unsigned char salt[SALT_BYTES];
        unsigned char expectedHash[HASH_BYTES];

        if (!fromHex(storedSalt, salt, sizeof(salt)) ||
            !fromHex(storedHash, expectedHash, sizeof(expectedHash)))
        {
            sodium_memzero(salt, sizeof(salt));
            sodium_memzero(expectedHash, sizeof(expectedHash));
            return false;
        }

        // Recalculate the hash using the password the user just entered and
        // the salt saved when the account was created.
        unsigned char calculatedHash[HASH_BYTES];

        const int status = crypto_pwhash(
            calculatedHash,
            sizeof(calculatedHash),
            password.data(),
            static_cast<unsigned long long>(password.size()),
            salt,
            OPS_LIMIT,
            MEM_LIMIT,
            crypto_pwhash_ALG_ARGON2ID13);

        // Do not compare buffers if hashing failed.
        if (status != 0)
        {
            sodium_memzero(salt, sizeof(salt));
            sodium_memzero(expectedHash, sizeof(expectedHash));
            sodium_memzero(calculatedHash, sizeof(calculatedHash));
            return false;
        }

        // Use constant-time comparison to reduce timing information leakage.
        const bool matches =
            sodium_memcmp(calculatedHash, expectedHash, sizeof(expectedHash)) == 0;

        // Clear temporary cryptographic data before returning.
        sodium_memzero(salt, sizeof(salt));
        sodium_memzero(expectedHash, sizeof(expectedHash));
        sodium_memzero(calculatedHash, sizeof(calculatedHash));

        return matches;
    }

    std::string PasswordHasher::toHex(
        const unsigned char *bytes,
        std::size_t length)
    {
        // Two hexadecimal characters represent each byte.
        std::string output(length * 2, '\0');

        sodium_bin2hex(output.data(), output.size() + 1, bytes, length);

        return output;
    }

    bool PasswordHasher::fromHex(
        const std::string &hex,
        unsigned char *output,
        std::size_t outputLength)
    {
        // The input must contain exactly two characters per output byte.
        if (hex.size() != outputLength * 2)
        {
            return false;
        }

        // sodium_hex2bin rejects invalid hexadecimal characters.
        std::size_t decodedLength = 0;

        const int status = sodium_hex2bin(
            output,
            outputLength,
            hex.data(),
            hex.size(),
            nullptr,
            &decodedLength,
            nullptr);

        return status == 0 && decodedLength == outputLength;
    }

} // namespace lms::services
