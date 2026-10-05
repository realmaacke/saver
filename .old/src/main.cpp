#include <iostream>
#include "Core/Core.hpp"
#include "Service.hpp"

/**
* Main function
* Important Service calls before user executes command.
* Core saver_core is the class holding the commands.
* */
int main(int argc, char** argv) { 
    Core saver_core;
    // Service calls order is important to maintain.

    // Create directories and files if it does not exists.
    // Also loads the config into memory.
    Service::instance().store().environment_setup();
    
    // Sets base url for saver-remote inside sender.
    Service::instance().send().setBaseUrl();

    // Ensures that logged in user, stays logged in.
    Service::instance().user().alreadyConnectedUser();

    // checks if user already stands in a project.
    Service::instance().proj().check_if_in_project();

    if (argc < 2) {
        std::cout << "Saver: unknown command" << std::endl;
        return 1;
    }

    return saver_core.execute(argv[1], argc - 1, argv + 1);
}