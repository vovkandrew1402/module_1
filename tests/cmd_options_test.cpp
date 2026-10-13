// #include "cmd_options.h"
// #include <gtest/gtest.h>
// #include <string>
// #include <vector>

// using CryptoGuard::ProgramOptions;

// TEST(CmdOptionsTest, HelpReturnFalseTest) {
//     const int argc = 2;
//     const char *argv[argc] = {"CryptoGuard.exe", "--help"};
//     ProgramOptions options;

//     EXPECT_EQ(options.Parse(argc, argv), false);
// }

// TEST(CmdOptionsTest, InvalidOperationTest) {
//     const int argc = 4;
//     const char *argv[argc] = {"CryptoGuard.exe", "qwerty", "--input", "test.txt"};
//     ProgramOptions options;

//     EXPECT_THROW(options.Parse(argc, argv), std::invalid_argument);
// }

// TEST(CmdOptionsTest, InvalidCommandTest) {
//     const int argc = 5;
//     const char *argv[argc] = {"CryptoGuard.exe", "--command", "qwerty", "--input", "test.txt"};
//     ProgramOptions options;

//     EXPECT_THROW(options.Parse(argc, argv), std::invalid_argument);
// }

// TEST(CmdOptionsTest, NoInputArgTest) {
//     const int argc = 4;
//     const char *argv[argc] = {"CryptoGuard.exe", "--command", "encrypt", "--input"};
//     ProgramOptions options;

//     EXPECT_THROW(options.Parse(argc, argv), std::invalid_argument);
// }

// TEST(CmdOptionsTest, InputArgIsEmptyTest) {
//     const int argc = 5;
//     const char *argv[argc] = {"CryptoGuard.exe", "--command", "encrypt", "--input", ""};
//     ProgramOptions options;

//     EXPECT_THROW(options.Parse(argc, argv), std::invalid_argument);
// }

// TEST(CmdOptionsTest, NoOutputArgTest) {
//     const int argc = 6;
//     const char *argv[argc] = {"CryptoGuard.exe", "--command", "encrypt", "--input", "test.txt", "--output"};
//     ProgramOptions options;

//     EXPECT_THROW(options.Parse(argc, argv), std::invalid_argument);
// }

// TEST(CmdOptionsTest, OutputArgIsEmptyTest) {
//     const int argc = 7;
//     const char *argv[argc] = {"CryptoGuard.exe", "--command", "encrypt", "--input", "test.txt", "--output", ""};
//     ProgramOptions options;

//     EXPECT_THROW(options.Parse(argc, argv), std::invalid_argument);
// }

// TEST(CmdOptionsTest, NoPasswordArgTest) {
//     const int argc = 8;
//     const char *argv[argc] = {"CryptoGuard.exe", "--command", "encrypt", "--input",
//                               "test.txt",        "--output",  "out.txt", "--password"};
//     ProgramOptions options;

//     EXPECT_THROW(options.Parse(argc, argv), std::invalid_argument);
// }

// TEST(CmdOptionsTest, PasswordArgIsEmptyTest) {
//     const int argc = 9;
//     const char *argv[argc] = {"CryptoGuard.exe", "--command", "encrypt",    "--input", "test.txt",
//                               "--output",        "out.txt",   "--password", ""};
//     ProgramOptions options;

//     EXPECT_THROW(options.Parse(argc, argv), std::invalid_argument);
// }

// TEST(CmdOptionsTest, NotEnoughArgsForEncryptTest) {
//     const int argc = 5;
//     const char *argv[argc] = {"CryptoGuard.exe", "--command", "encrypt", "--input", "test.txt"};
//     ProgramOptions options;

//     EXPECT_THROW(options.Parse(argc, argv), std::invalid_argument);
// }

// TEST(CmdOptionsTest, NotEnoughArgsForDecryptTest) {
//     const int argc = 5;
//     const char *argv[argc] = {"CryptoGuard.exe", "--command", "decrypt", "--input", "test.txt"};
//     ProgramOptions options;

//     EXPECT_THROW(options.Parse(argc, argv), std::invalid_argument);
// }

// TEST(CmdOptionsTest, NotEnoughArgsForChecksumTest) {
//     const int argc = 3;
//     const char *argv[argc] = {"CryptoGuard.exe", "--command", "checksum"};
//     ProgramOptions options;

//     EXPECT_THROW(options.Parse(argc, argv), std::invalid_argument);
// }

// TEST(CmdOptionsTest, HasPasswordForChecksumTest) {
//     const int argc = 7;
//     const char *argv[argc] = {"CryptoGuard.exe", "--command", "checksum", "--input", "test.txt", "--password",
//     "123"}; ProgramOptions options;

//     EXPECT_THROW(options.Parse(argc, argv), std::invalid_argument);
// }

// TEST(CmdOptionsTest, HasOutputFileForChecksumTest) {
//     const int argc = 7;
//     const char *argv[argc] = {"CryptoGuard.exe", "--command", "checksum", "--input", "test.txt", "--output",
//     "out.txt"}; ProgramOptions options;

//     EXPECT_THROW(options.Parse(argc, argv), std::invalid_argument);
// }

// TEST(CmdOptionsTest, HasOutputFileAndPasswordForChecksumTest) {
//     const int argc = 9;
//     const char *argv[argc] = {"CryptoGuard.exe", "--command", "checksum",   "--input", "test.txt",
//                               "--output",        "out.txt",   "--password", "123"};
//     ProgramOptions options;

//     EXPECT_THROW(options.Parse(argc, argv), std::invalid_argument);
// }

// TEST(CmdOptionsTest, CorrectEncryptCommandTest) {
//     const int argc = 9;
//     const char *argv[argc] = {"CryptoGuard.exe", "--command", "encrypt",    "--input", "test.txt",
//                               "--output",        "out.txt",   "--password", "123"};
//     ProgramOptions options;

//     EXPECT_EQ(options.Parse(argc, argv), true);
// }

// TEST(CmdOptionsTest, CorrectDecryptCommandTest) {
//     const int argc = 9;
//     const char *argv[argc] = {"CryptoGuard.exe", "--command", "decrypt",    "--input", "test.txt",
//                               "--output",        "out.txt",   "--password", "123"};
//     ProgramOptions options;

//     EXPECT_EQ(options.Parse(argc, argv), true);
// }

// TEST(CmdOptionsTest, CorrectChecksumCommandTest) {
//     const int argc = 5;
//     const char *argv[argc] = {"CryptoGuard.exe", "--command", "checksum", "--input", "test.txt"};
//     ProgramOptions options;

//     EXPECT_EQ(options.Parse(argc, argv), true);
// }

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