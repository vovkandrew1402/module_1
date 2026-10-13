#include "crypto_guard_ctx.h"

#include <gtest/gtest.h>

#include <sstream>
#include <stdexcept>
#include <string>

namespace CryptoGuard { namespace {

class CryptoGuardCtxTest : public ::testing::Test {
protected:
    CryptoGuardCtx cryptoCtx;

    std::string Encrypt(const std::string &plaintext, const std::string &password = "qwerty") {
        std::stringstream input{plaintext};
        std::stringstream output;

        cryptoCtx.EncryptFile(input, output, password);

        return output.str();
    }

    std::string Decrypt(const std::string &ciphertext, const std::string &password = "qwerty") {
        std::stringstream input{ciphertext};
        std::stringstream output;

        cryptoCtx.DecryptFile(input, output, password);

        return output.str();
    }
};

TEST_F(CryptoGuardCtxTest, EncryptNormalText) {
    const std::string plaintext = "Hello, CryptoGuard!";

    const std::string ciphertext = Encrypt(plaintext);

    EXPECT_FALSE(ciphertext.empty());
    EXPECT_NE(ciphertext, plaintext);
}

TEST_F(CryptoGuardCtxTest, EncryptEmptyInput) {
    const std::string ciphertext = Encrypt("");

    EXPECT_FALSE(ciphertext.empty());
}

TEST_F(CryptoGuardCtxTest, EncryptLargeInput) {
    const std::string plaintext(100'000, 'A');

    const std::string ciphertext = Encrypt(plaintext);

    EXPECT_FALSE(ciphertext.empty());
    EXPECT_NE(ciphertext, plaintext);
}

TEST_F(CryptoGuardCtxTest, EncryptBinaryData) {
    const std::string plaintext{
        '\0', '\1', '\2', '\0', static_cast<char>(0x7F), static_cast<char>(0x80), static_cast<char>(0xFF)};

    const std::string ciphertext = Encrypt(plaintext);

    EXPECT_FALSE(ciphertext.empty());
    EXPECT_NE(ciphertext, plaintext);
}

TEST_F(CryptoGuardCtxTest, EncryptInputStreamInBadStateThrows) {
    std::stringstream input{"some input"};
    std::stringstream output;

    input.setstate(std::ios::badbit);

    ASSERT_THROW(cryptoCtx.EncryptFile(input, output, "qwerty"), std::runtime_error);
}

TEST_F(CryptoGuardCtxTest, EncryptOutputStreamInBadStateThrows) {
    std::stringstream input{"some input"};
    std::stringstream output;

    output.setstate(std::ios::badbit);

    ASSERT_THROW(cryptoCtx.EncryptFile(input, output, "qwerty"), std::runtime_error);
}

TEST_F(CryptoGuardCtxTest, EncryptInvalidInputDoesNotProduceSuccess) {
    std::stringstream input{"some input"};
    std::stringstream output;

    input.setstate(std::ios::badbit);

    EXPECT_THROW(cryptoCtx.EncryptFile(input, output, "qwerty"), std::runtime_error);

    EXPECT_TRUE(output.str().empty());
}

TEST_F(CryptoGuardCtxTest, DecryptRoundTrip) {
    const std::string plaintext = "The quick brown fox jumps over the lazy dog.";

    const std::string ciphertext = Encrypt(plaintext);
    const std::string decrypted = Decrypt(ciphertext);

    EXPECT_EQ(decrypted, plaintext);
}

TEST_F(CryptoGuardCtxTest, DecryptEmptyInput) {
    const std::string ciphertext = Encrypt("");
    const std::string decrypted = Decrypt(ciphertext);

    EXPECT_TRUE(decrypted.empty());
}

TEST_F(CryptoGuardCtxTest, DecryptLargeInput) {
    const std::string plaintext(100'000, 'B');

    const std::string ciphertext = Encrypt(plaintext);
    const std::string decrypted = Decrypt(ciphertext);

    EXPECT_EQ(decrypted, plaintext);
}

TEST_F(CryptoGuardCtxTest, DecryptBinaryData) {
    const std::string plaintext{
        '\0', '\1', '\2', '\0', static_cast<char>(0x7F), static_cast<char>(0x80), static_cast<char>(0xFF)};

    const std::string ciphertext = Encrypt(plaintext);
    const std::string decrypted = Decrypt(ciphertext);

    EXPECT_EQ(decrypted, plaintext);
}

TEST_F(CryptoGuardCtxTest, DecryptInputStreamInBadStateThrows) {
    std::stringstream input{"some ciphertext"};
    std::stringstream output;

    input.setstate(std::ios::badbit);

    ASSERT_THROW(cryptoCtx.DecryptFile(input, output, "qwerty"), std::runtime_error);
}

TEST_F(CryptoGuardCtxTest, DecryptOutputStreamInBadStateThrows) {
    const std::string ciphertext = Encrypt("some input");

    std::stringstream input{ciphertext};
    std::stringstream output;

    output.setstate(std::ios::badbit);

    ASSERT_THROW(cryptoCtx.DecryptFile(input, output, "qwerty"), std::runtime_error);
}

TEST_F(CryptoGuardCtxTest, DecryptEmptyCiphertextThrows) {
    std::stringstream input;
    std::stringstream output;

    ASSERT_THROW(cryptoCtx.DecryptFile(input, output, "qwerty"), std::runtime_error);
}

TEST_F(CryptoGuardCtxTest, CanEncryptAndDecryptMultipleTimes) {
    const std::string first = "first message";
    const std::string second = "second message";

    const std::string encryptedFirst = Encrypt(first);
    const std::string encryptedSecond = Encrypt(second);

    EXPECT_EQ(Decrypt(encryptedFirst), first);
    EXPECT_EQ(Decrypt(encryptedSecond), second);
}

TEST_F(CryptoGuardCtxTest, SeparateContextsCanProcessDifferentMessages) {
    CryptoGuardCtx firstCtx;
    CryptoGuardCtx secondCtx;

    std::stringstream firstInput{"first"};
    std::stringstream firstEncrypted;

    firstCtx.EncryptFile(firstInput, firstEncrypted, "first-password");

    std::stringstream secondInput{"second"};
    std::stringstream secondEncrypted;

    secondCtx.EncryptFile(secondInput, secondEncrypted, "second-password");

    std::stringstream firstCiphertext{firstEncrypted.str()};
    std::stringstream firstDecrypted;

    firstCtx.DecryptFile(firstCiphertext, firstDecrypted, "first-password");

    EXPECT_EQ(firstDecrypted.str(), "first");
}

}}  // namespace CryptoGuard