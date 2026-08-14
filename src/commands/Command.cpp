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
*/
cmd Command::project() {};

/**
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
            return 0;
        }

        if (!std::filesystem::is_directory(".")) {
            Output::error("Not in a valid direcory.");
            return 1;
        }
        Service::instance().proj().set_root(".");
        return 0;
    };
}

/**
* Method that adds files to cache.
*/
cmd Command::add(){
    return [](int argc, char** argv) {
        if (argc > 1) {
            Service::instance().proj().populate_cache(argv[1]);
            return 0;
        }
        Output::error("You need to specify a path.");
        return 1;
    };
};

/**
* Method that removes files from cache
*/
cmd Command::remove(){};


/**
* Method that resets the cache
*/
cmd Command::reset(){};


/**
* Method that describes the cache.
*
*/
cmd Command::describe(){};


/**
* Method that uploads the cache.
* In order to upload,
*   the cache will need to be described first.
*/
cmd Command::upload(){};


/**
*   Method that downloads a project.
*/
cmd Command::download(){};
