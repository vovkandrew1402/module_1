#include "cmd_options.h"
#include <iostream>
#include <print>
#include <stdexcept>

namespace CryptoGuard {

namespace po = boost::program_options;

ProgramOptions::ProgramOptions() : desc_("Allowed options") {
    desc_.add_options()("help,h", "help info")("command,c", po::value<std::string>(), "command info")(
        "input,i", po::value<std::string>(), "input filename")("output,o", po::value<std::string>(), "output filename")(
        "password,p", po::value<std::string>(), "password info");
}

ProgramOptions::~ProgramOptions() = default;

bool ProgramOptions::Parse(int argc, char *argv[]) {

    po::store(po::command_line_parser(argc, argv).options(desc_).run(), vm_);
    if (vm_.count("help")) {
        std::cout << desc_ << std::endl;
        return false;
    }
    po::notify(vm_);
    if (!vm_.count("command")) {
        throw std::invalid_argument("Use flag --command or -c for command");
    }
    const auto &command = vm_.at("command").as<std::string>();

    const auto it = commandMapping_.find(command);

    if (it == commandMapping_.end()) {
        throw std::invalid_argument(std::format("Invalid command {}", command));
    }
    command_ = it->second;
    if (!vm_.count("input")) {
        throw std::invalid_argument("No filename for --input/-i");
    }
    inputFile_ = vm_["input"].as<std::string>();
    if (inputFile_.empty()) {
        throw std::invalid_argument("Unable to use empty input filename");
    }

    switch (command_) {
    case COMMAND_TYPE::ENCRYPT:
    case COMMAND_TYPE::DECRYPT:
        if (!vm_.count("output")) {
            throw std::invalid_argument("No filename for --output/-o");
        }
        outputFile_ = vm_["output"].as<std::string>();
        if (outputFile_.empty()) {
            throw std::invalid_argument("Unable to use empty output filename");
        }
        if (!vm_.count("password")) {
            throw std::invalid_argument("No password for --password/-p");
        }
        password_ = vm_["password"].as<std::string>();
        if (password_.empty()) {
            throw std::invalid_argument("Unable to use empty password");
        }
        break;

    case COMMAND_TYPE::CHECKSUM:
        if (vm_.count("output") or vm_.count("password")) {
            throw std::invalid_argument("Unable to use --output and --password for checksum");
        }
        break;

    default:
        throw std::runtime_error{"Unsupported command"};
    }
    return true;
}

}  // namespace CryptoGuard
