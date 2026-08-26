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
        Output::print("[General Commands]");
        Output::print("help     - Displays all available commands \n");

        Output::print("[Project Specific Commands]");
        Output::print("saver project <path/to/root> - Displays information about the current project");
        Output::print("saver start <path/to/root> - Starts a new project");
        Output::print("saver add <path/to/file(s) - Adds file(s) to cache");
        Output::print("saver remove <path/to/file(s) - Removes file(s) from cache");
        Output::print("saver describe <message> - Describes the changes made in cache");
        Output::print("saver upload ? - uploads the cache");
        Output::print("saver download <username/project_name> - Downloads a specific project \n");

        Output::print("saver connect <username> <password> - Connects to the remote");
        Output::print("saver disconnect - Disconnects from the remote");

        return 0;
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
        switch (argc) {
            case 3:
                return Service::instance()
                    .proj()
                    .create_new_project(
                    argv[1],
                    argv[2]
                    );
            break;
            case 2:
                return Service::instance()
                    .proj()
                    .create_new_project(argv[1]);
            break;
        }

        if (!std::filesystem::is_directory(".")) {
            Output::error("Not in a valid direcory.");
            return 1;
        }
        return Service::instance().proj().create_new_project(".");
    };
}

/**
* Method that adds files to cache.
*/
cmd Command::add(){
    return [](int argc, char** argv) {
        if (argc > 1) {
            // Service::instance().proj().populate_cache(argv[1]);
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
        // Service::instance().proj().describe_cache(describe_message);
        return 0;
    };
};


/**
* Method that resets the cache
*/
cmd Command::reset(){
    return [](int, char**) {
        Output::print("Cache has been cleared");
        // return Service::instance().proj().reset_cache();
        return 0;
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
        // Service::instance().proj().describe_cache(describe_message);
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

cmd Command::disconnect() {
    return [](int, char**) {
        return Service::instance().user().disconnectUser();
    };
}