#include "Commands.hpp"
#include "Output/Codes/InformationCodes.hpp"
#include "Output/Output.hpp"
#include "Service.hpp"
#include <cstddef>
#include <string>

cmd Commands::help() {
    return [](int, char**) {
        return Service::instance().proj().project_help();
    };
};

cmd Commands::project() {
    return [](int, char**) {
        return Service::instance().proj().project_status();
    };
};

cmd Commands::start() {
    return [](int, char**) {
        return Service::instance().proj().project_start();
    };
};

cmd Commands::add() {
    return [](int argc, char** argv) {
        
        if (argc < 2) {
            Output::info(InfoCode::NO_PATH_SPECIFIED);
            return 0;
        }

        if (!Service::instance().proj().inside_project()) {
            Output::info(InfoCode::PROJECT_NOT_FOUND);
            return 0;
        }
      
        return Service::instance().proj().project_add(argv[1]);
    };
};

cmd Commands::remove() {
    return [](int argc, char** argv) {
        if (argc < 2) {
            Output::info(InfoCode::NO_PATH_SPECIFIED);
            return 0;
        }
        return Service::instance().proj().project_removal(argv[1]);
    };
};

cmd Commands::describe() {
    return [](int argc, char** argv) {
        if (argc < 2) {
            Output::info(InfoCode::NO_DESCRIBE_WITH_COMMAND);
            return 0;
        }
        return Service::instance().proj().project_describe(argv[1]);
    };
};

cmd Commands::upload() {
    return [](int, char**) {
        return Service::instance().proj().project_upload();
    };
};

cmd Commands::download() {
    return 0;
};

cmd Commands::connect() {
    return [] (int, char**) {
        return Service::instance().usr().connectUser();
    };
};

cmd Commands::disconnect() {
    return [] (int ,char**) {
        return Service::instance().usr().disconnectUser();
    };
};
