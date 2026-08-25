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
        Service::instance().store().addToConfig("API_PORT", "8080");
        Output::print("Help is here!");
        return 1;
    };
}

/**
*
*/
cmd Command::project() {
    return [](int, char**) {
        return 0;
    };
};

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
cmd Command::remove(){
    return [](int argc, char** argv) {
        if (argc < 2) {
            Output::print("You need to specify path to remove");
            Output::print("Syntax: saver remove path/to/file");
            return 1;
        }
        std::string describe_message = argv[1];
        Service::instance().proj().describe_cache(describe_message);
        return 0;
    };
};


/**
* Method that resets the cache
*/
cmd Command::reset(){
    return [](int, char**) {
        Output::print("Cache has been cleared");
        return Service::instance().proj().reset_cache();
    };
};


/**
* Method that describes the cache.
*
*/
cmd Command::describe(){
    return [](int argc, char** argv) {
        if (argc < 2) {
            Output::print("You need to describe the changes.");
            Output::print("Syntax: saver \"describe here\"");
            return 1;
        }
        std::string describe_message = argv[1];
        Service::instance().proj().describe_cache(describe_message);
        return 0;
    };
};


/**
* Method that uploads the cache.
* In order to upload,
*   the cache will need to be described first.
*/
cmd Command::upload(){
    return [](int, char**) {
        return 0;
    };
};


/**
*   Method that downloads a project.
*/
cmd Command::download(){
    return [](int, char**) {
        return 0;
    };
};


cmd Command::login() {
    return [](int argc, char** argv) {
        if (argc < 3) {
            Output::print("Invalid use of command");
            Output::print("Syntax: login <username> <password>");
            return 0;
        }
        return Service::instance().user().connectUser(argv[1], argv[2]);
    };
};