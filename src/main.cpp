#include <iostream>
#include "Core/Core.hpp"
#include "Service.hpp"

/**
* Fill this out.
* */
int main(int argc, char** argv) { 
    Core saver_core;

    // Internal config
    Service::instance().store().createConfigDirectory();
    if (Service::instance().store().createConfigFile()) {
        Service::instance().store().loadConfig();
    }

    // Ensures that logged in user, stays logged in.
    Service::instance().user().alreadyConnectedUser();

    // checks if user already stands in a project.
    Service::instance().proj().check_if_in_project();

    if (argc < 2) {
        std::cout << "Saver: unknown command" << std::endl;
        return 1;
    }

    return saver_core.execute(argv[1], argc - 1, argv + 1);
    // return saver.executeCommand(argv[1], argc - 1, argv + 1);
}