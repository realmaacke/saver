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
* P
* */
void Core::populate_cmd_list() {
    this->cmd_list = {
        {"help", Command::help()},
        {"start", Command::start()}
    };
}

