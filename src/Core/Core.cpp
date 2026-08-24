#include <iostream>
#include "Core.hpp"
#include "Command.hpp"
#include <functional>

Core::Core() {
    this->populate_cmd_list();
}

/**
* Iterats through the unordered list to find the command.
*/
int Core::execute(const std::string& name, int argc, char** argv) {
    auto command = this->cmd_list.find(name);

    if (command == this->cmd_list.end()) {
        std::cout << "Saver: unknown command" << std::endl;
        return 1;
    }
    return command->second(argc, argv);
}

/**
* Populates cmd list.
* */
void Core::populate_cmd_list() {
    this->cmd_list = {
        {"help", Command::help()},
        {"project", Command::project()},

        {"start", Command::start()},
        {"add", Command::add()},
        {"remove", Command::remove()},
        {"reset", Command::reset()},
        {"describe", Command::describe()},
        {"upload", Command::upload()},
        {"download", Command::download()},

        {"login", Command::login()}
    };
}

