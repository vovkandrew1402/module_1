#include "crypto_guard_ctx.h"
#include <algorithm>
#include <array>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <openssl/evp.h>
#include <print>
#include <sstream>
#include <stdexcept>
#include <string>

namespace {
struct AesCipherParams {
    static const size_t KEY_SIZE = 32;
    static const size_t IV_SIZE = 16;
    const EVP_CIPHER *cipher = EVP_aes_256_cbc();

    int encrypt;
    std::array<unsigned char, KEY_SIZE> key;
    std::array<unsigned char, IV_SIZE> iv;
};

struct EvpMdCtxDeleter {
    void operator()(EVP_MD_CTX *ctx) const noexcept { EVP_MD_CTX_free(ctx); }
};
struct EvpCipherCtxDeleter {
    void operator()(EVP_CIPHER_CTX *ctx) const noexcept { EVP_CIPHER_CTX_free(ctx); }
};

}  // namespace

namespace CryptoGuard {

class CryptoGuardCtx::Impl {
private:
    void doCrypt(std::iostream &inStream, std::iostream &outStream, std::string_view password, bool encrypt) {
        if (!inStream) {
            throw std::runtime_error{"Input stream is not ready"};
        }

        if (!outStream) {
            throw std::runtime_error{"Output stream is not ready"};
        }
        auto params = CreateChiperParamsFromPassword(password);
        using CipherCtxPtr = std::unique_ptr<EVP_CIPHER_CTX, EvpCipherCtxDeleter>;

        CipherCtxPtr ctx{EVP_CIPHER_CTX_new()};

        if (!ctx) {
            throw std::runtime_error{"Failed to create cipher context"};
        }

        if (EVP_CipherInit_ex(ctx.get(), params.cipher, nullptr, params.key.data(), params.iv.data(),
                              encrypt ? 1 : 0) != 1) {
            throw std::runtime_error{"Failed to initialize cipher"};
        }

        std::array<unsigned char, 1024> inBuf{};
        std::array<unsigned char, 1024 + EVP_MAX_BLOCK_LENGTH> outBuf{};

        while (true) {
            inStream.read(reinterpret_cast<char *>(inBuf.data()), static_cast<std::streamsize>(inBuf.size()));

            const std::streamsize bytesRead = inStream.gcount();

            if (bytesRead > 0) {
                int outLen = 0;

                if (EVP_CipherUpdate(ctx.get(), outBuf.data(), &outLen, inBuf.data(), static_cast<int>(bytesRead)) !=
                    1) {
                    throw std::runtime_error{"Cipher update failed"};
                }

                outStream.write(reinterpret_cast<const char *>(outBuf.data()), outLen);

                if (!outStream) {
                    throw std::runtime_error{"Failed to write output"};
                }
            }

            if (inStream.eof()) {
                break;
            }

            if (!inStream) {
                throw std::runtime_error{"Failed to read input"};
            }
        }

        int finalLen = 0;

        if (EVP_CipherFinal_ex(ctx.get(), outBuf.data(), &finalLen) != 1) {
            throw std::runtime_error{"Cipher finalization failed"};
        }

        outStream.write(reinterpret_cast<const char *>(outBuf.data()), finalLen);

        if (!outStream) {
            throw std::runtime_error{"Failed to write final output"};
        }
    }

public:
    void EncryptFileImpl(std::iostream &inStream, std::iostream &outStream, std::string_view password) {
        doCrypt(inStream, outStream, password, true);
    }
    void DecryptFileImpl(std::iostream &inStream, std::iostream &outStream, std::string_view password) {
        doCrypt(inStream, outStream, password, false);
    }
    std::string CalculateChecksumImpl(std::iostream &inStream) {
        if (!inStream) {
            throw std::runtime_error{"Input stream is not ready"};
        }

        using CtxPtr = std::unique_ptr<EVP_MD_CTX, EvpMdCtxDeleter>;

        CtxPtr ctx{EVP_MD_CTX_new()};

        if (!ctx) {
            throw std::runtime_error{"Failed to create digest context"};
        }

        if (EVP_DigestInit_ex(ctx.get(), EVP_sha256(), nullptr) != 1) {
            throw std::runtime_error{"Failed to initialize SHA-256"};
        }

        std::array<unsigned char, 8192> buffer{};

        while (true) {
            inStream.read(reinterpret_cast<char *>(buffer.data()), static_cast<std::streamsize>(buffer.size()));

            const auto bytesRead = inStream.gcount();

            if (bytesRead > 0) {
                if (EVP_DigestUpdate(ctx.get(), buffer.data(), static_cast<std::size_t>(bytesRead)) != 1) {
                    throw std::runtime_error{"Failed to update SHA-256"};
                }
            }

            if (inStream.bad()) {
                throw std::runtime_error{"Failed to read input stream"};
            }

            if (inStream.eof()) {
                break;
            }

            if (inStream.fail()) {
                throw std::runtime_error{"Failed to read input stream"};
            }
        }
        std::array<unsigned char, EVP_MAX_MD_SIZE> digest{};
        unsigned int digestLength = 0;

        if (EVP_DigestFinal_ex(ctx.get(), digest.data(), &digestLength) != 1) {
            throw std::runtime_error{"Failed to finalize SHA-256"};
        }

        std::stringstream checksumStream;

        checksumStream << std::hex << std::nouppercase << std::setfill('0');

        for (unsigned int i = 0; i < digestLength; ++i) {
            checksumStream << std::setw(2) << static_cast<unsigned int>(digest[i]);

            if (!checksumStream) {
                throw std::runtime_error("Failed to format checksum");
            }
        }

        return checksumStream.str();
    }
    AesCipherParams CreateChiperParamsFromPassword(std::string_view password) {
        AesCipherParams params;
        constexpr std::array<unsigned char, 8> salt = {'1', '2', '3', '4', '5', '6', '7', '8'};

        int result = EVP_BytesToKey(params.cipher, EVP_sha256(), salt.data(),
                                    reinterpret_cast<const unsigned char *>(password.data()), password.size(), 1,
                                    params.key.data(), params.iv.data());

        if (result == 0) {
            throw std::runtime_error{"Failed to create a key from password"};
        }

        return params;
    }
};

CryptoGuardCtx::CryptoGuardCtx() : pImpl_(std::make_unique<Impl>()) {}

CryptoGuardCtx::~CryptoGuardCtx() = default;

CryptoGuardCtx::CryptoGuardCtx(CryptoGuardCtx &&) noexcept = default;
CryptoGuardCtx &CryptoGuardCtx::operator=(CryptoGuardCtx &&) noexcept = default;

void CryptoGuardCtx::EncryptFile(std::iostream &inStream, std::iostream &outStream, std::string_view password) {
    pImpl_->EncryptFileImpl(inStream, outStream, password);
}
void CryptoGuardCtx::DecryptFile(std::iostream &inStream, std::iostream &outStream, std::string_view password) {
    pImpl_->DecryptFileImpl(inStream, outStream, password);
}
std::string CryptoGuardCtx::CalculateChecksum(std::iostream &inStream) {
    return pImpl_->CalculateChecksumImpl(inStream);
}

}  // namespace CryptoGuard
