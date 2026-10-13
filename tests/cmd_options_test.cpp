#include "cmd_options.h"

#include <gtest/gtest.h>

#include <initializer_list>
#include <stdexcept>
#include <string>
#include <vector>

using CryptoGuard::ProgramOptions;

namespace {

bool ParseArgs(ProgramOptions &options, std::initializer_list<std::string> arguments) {
    std::vector<std::string> strings;
    strings.emplace_back("CryptoGuard");

    for (const auto &arg : arguments) {
        strings.push_back(arg);
    }

    std::vector<char *> argv;
    for (auto &arg : strings) {
        argv.push_back(arg.data());
    }

    return options.Parse(static_cast<int>(argv.size()), argv.data());
}

}  // namespace

TEST(CmdOptionsTest, HelpReturnsFalse) {
    ProgramOptions options;

    EXPECT_FALSE(ParseArgs(options, {"--help"}));
}

TEST(CmdOptionsTest, MissingCommandThrows) {
    ProgramOptions options;

    EXPECT_THROW(ParseArgs(options, {"--input", "test.txt"}), std::invalid_argument);
}

TEST(CmdOptionsTest, InvalidCommandThrows) {
    ProgramOptions options;

    EXPECT_THROW(ParseArgs(options, {"--command", "qwerty", "--input", "test.txt"}), std::invalid_argument);
}

TEST(CmdOptionsTest, MissingInputThrows) {
    ProgramOptions options;

    EXPECT_THROW(ParseArgs(options, {"--command", "encrypt"}), std::invalid_argument);
}

TEST(CmdOptionsTest, InputWithoutValueThrows) {
    ProgramOptions options;

    EXPECT_THROW(ParseArgs(options, {"--command", "encrypt", "--input"}), std::exception);
}

TEST(CmdOptionsTest, EmptyInputThrows) {
    ProgramOptions options;

    EXPECT_THROW(
        ParseArgs(options, {"--command", "encrypt", "--input", "", "--output", "out.txt", "--password", "qwerty"}),
        std::invalid_argument);
}

TEST(CmdOptionsTest, MissingOutputThrows) {
    ProgramOptions options;

    EXPECT_THROW(ParseArgs(options, {"--command", "encrypt", "--input", "test.txt"}), std::invalid_argument);
}

TEST(CmdOptionsTest, EmptyOutputThrows) {
    ProgramOptions options;

    EXPECT_THROW(
        ParseArgs(options, {"--command", "encrypt", "--input", "test.txt", "--output", "", "--password", "qwerty"}),
        std::invalid_argument);
}

TEST(CmdOptionsTest, MissingPasswordThrows) {
    ProgramOptions options;

    EXPECT_THROW(ParseArgs(options, {"--command", "encrypt", "--input", "test.txt", "--output", "out.txt"}),
                 std::invalid_argument);
}

TEST(CmdOptionsTest, PasswordWithoutValueThrows) {
    ProgramOptions options;

    EXPECT_THROW(
        ParseArgs(options, {"--command", "encrypt", "--input", "test.txt", "--output", "out.txt", "--password"}),
        std::exception);
}

TEST(CmdOptionsTest, EmptyPasswordThrows) {
    ProgramOptions options;

    EXPECT_THROW(
        ParseArgs(options, {"--command", "encrypt", "--input", "test.txt", "--output", "out.txt", "--password", ""}),
        std::invalid_argument);
}

TEST(CmdOptionsTest, DecryptRequiresOutputAndPassword) {
    ProgramOptions options;

    EXPECT_THROW(ParseArgs(options, {"--command", "decrypt", "--input", "test.txt"}), std::invalid_argument);
}

TEST(CmdOptionsTest, ChecksumRequiresInput) {
    ProgramOptions options;

    EXPECT_THROW(ParseArgs(options, {"--command", "checksum"}), std::invalid_argument);
}

TEST(CmdOptionsTest, ChecksumRejectsPassword) {
    ProgramOptions options;

    EXPECT_THROW(ParseArgs(options, {"--command", "checksum", "--input", "test.txt", "--password", "qwerty"}),
                 std::invalid_argument);
}

TEST(CmdOptionsTest, ChecksumRejectsOutput) {
    ProgramOptions options;

    EXPECT_THROW(ParseArgs(options, {"--command", "checksum", "--input", "test.txt", "--output", "out.txt"}),
                 std::invalid_argument);
}

TEST(CmdOptionsTest, CorrectEncryptCommand) {
    ProgramOptions options;

    ASSERT_TRUE(ParseArgs(
        options, {"--command", "encrypt", "--input", "test.txt", "--output", "out.txt", "--password", "qwerty"}));

    EXPECT_EQ(options.GetCommand(), ProgramOptions::COMMAND_TYPE::ENCRYPT);
    EXPECT_EQ(options.GetInputFile(), "test.txt");
    EXPECT_EQ(options.GetOutputFile(), "out.txt");
    EXPECT_EQ(options.GetPassword(), "qwerty");
}

TEST(CmdOptionsTest, CorrectDecryptCommand) {
    ProgramOptions options;

    ASSERT_TRUE(ParseArgs(
        options, {"--command", "decrypt", "--input", "test.txt", "--output", "out.txt", "--password", "qwerty"}));

    EXPECT_EQ(options.GetCommand(), ProgramOptions::COMMAND_TYPE::DECRYPT);
}

TEST(CmdOptionsTest, CorrectChecksumCommand) {
    ProgramOptions options;

    ASSERT_TRUE(ParseArgs(options, {"--command", "checksum", "--input", "test.txt"}));

    EXPECT_EQ(options.GetCommand(), ProgramOptions::COMMAND_TYPE::CHECKSUM);
    EXPECT_EQ(options.GetInputFile(), "test.txt");
}