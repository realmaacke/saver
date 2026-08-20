#include <cstddef>
#include <functional>
#include <string>
#include <filesystem>

#include "Command.hpp"
#include "Service.hpp"
#include "Output/Output.hpp"

// Uses cmd as return to return lambda
namespace fs = std::filesystem;
using cmd = std::function<int(int, char **)>;

/**
*   Displays helpful information
* */
cmd Command::help() {
    return [](int, char**) {
        Output::print("Help is here!");
        return 1;
    };
}

/**
*
*
*/
cmd Command::start() {
    return [](int argc, char** argv) {
        if (argc > 1) {
            if (!fs::is_directory(argv[1])) {
                Output::error("The path given is not valid.");
                return 1;
            }
            Service::instance().proj().set_root(argv[1]);
            Service::instance().proj().create_project(argv[1]);
            return 0;
        }
        
        if (!std::filesystem::is_directory(".")) {
            Output::error("Not in a valid direcory.");
            return 1;
        }
        Service::instance().proj().set_root(".");
        Service::instance().proj().create_project("");
        return 0;
    };
}