#include <unordered_map>
#include <functional>

#include "Commands.hpp"
#include "Output/Codes/InformationCodes.hpp"
#include "Output/Output.hpp"
#include "Service.hpp"

std::unordered_map<std::string, std::function<int(int, char**)> (*)()>
commands = {
    {"help",        Commands::help},
    {"project",     Commands::project},

    {"start",       Commands::start},
    {"add",         Commands::add},
    {"remove",      Commands::remove},

    {"describe",    Commands::describe},
    {"upload",      Commands::upload},
    {"download",    Commands::download},

    {"connect",     Commands::connect},
    {"disconnect",  Commands::disconnect},

    // add defaults argv[1] = to change
    // {"default.remote", Command::defaults_remote()}
};

int execute(const std::string& name, int argc, char** argv) {
    auto cmd_call = commands.find(name);

    if (cmd_call == commands.end()) {
        Output::info(InfoCode::UNKNOWN_COMMAND);
        return 1;
    }
    return cmd_call->second()(argc, argv);
}

int main(int argc, char** argv) {
    if (argc < 2) {
        Output::info(InfoCode::UNKNOWN_COMMAND);
        return 1;
    }

    return execute(argv[1], argc - 1, argv + 1);
}