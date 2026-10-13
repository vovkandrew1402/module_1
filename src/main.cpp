
#include "cmd_options.h"
#include "crypto_guard_ctx.h"

#include <fstream>
#include <iostream>
#include <print>
#include <stdexcept>
#include <string>

int main(int argc, char *argv[]) {
    try {
        CryptoGuard::ProgramOptions options;

        if (!options.Parse(argc, argv)) {
            return 0;
        }

        CryptoGuard::CryptoGuardCtx cryptoCtx;

        using COMMAND_TYPE = CryptoGuard::ProgramOptions::COMMAND_TYPE;

        const std::string inputPath = options.GetInputFile();

        std::fstream inStream{inputPath, std::ios::in | std::ios::binary};

        if (!inStream) {
            throw std::runtime_error{"Failed to open input file"};
        }

        switch (options.GetCommand()) {
        case COMMAND_TYPE::ENCRYPT: {
            const std::string outputPath = options.GetOutputFile();

            if (inputPath == outputPath) {
                throw std::runtime_error{"Input and output files must be different"};
            }

            std::fstream outStream{outputPath, std::ios::out | std::ios::binary | std::ios::trunc};

            if (!outStream) {
                throw std::runtime_error{"Failed to open output file"};
            }

            cryptoCtx.EncryptFile(inStream, outStream, options.GetPassword());

            outStream.flush();

            if (!outStream) {
                throw std::runtime_error{"Failed to write encrypted file"};
            }

            std::print("File encoded successfully\n");
            break;
        }

        case COMMAND_TYPE::DECRYPT: {
            const std::string outputPath = options.GetOutputFile();

            if (inputPath == outputPath) {
                throw std::runtime_error{"Input and output files must be different"};
            }

            std::fstream outStream{outputPath, std::ios::out | std::ios::binary | std::ios::trunc};

            if (!outStream) {
                throw std::runtime_error{"Failed to open output file"};
            }

            cryptoCtx.DecryptFile(inStream, outStream, options.GetPassword());

            outStream.flush();

            if (!outStream) {
                throw std::runtime_error{"Failed to write decrypted file"};
            }

            std::print("File decoded successfully\n");
            break;
        }

        case COMMAND_TYPE::CHECKSUM: {
            const std::string checksum = cryptoCtx.CalculateChecksum(inStream);

            std::print("Checksum: {}\n", checksum);
            break;
        }

        default:
            throw std::runtime_error{"Unsupported command"};
        }

    } catch (const std::exception &e) {
        std::print(std::cerr, "Error: {}\n", e.what());
        return 1;
    }

    return 0;
}