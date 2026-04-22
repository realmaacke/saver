#include <iostream>

#include <unordered_map>
#include <functional>
#include <string>

#include "saverService.hpp"
#include "commandFactory.hpp"

int SaverService::executeCommand(const std::string& name, int argc, char** argv) {
    auto it = this->commands.find(name);

    if (it == this->commands.end()) {
        std::cerr << "saver: unknown command" << std::endl;
        return 1;
    }
    return it->second(argc, argv);
}

void SaverService::loadCommands() {

    this->commands = {
        { "init", CommandFactory::init(*this)},
        { "status", CommandFactory::status(*this) },
        { "add", CommandFactory::add(*this) },
        { "describe", CommandFactory::describe(*this)},
        { "save", CommandFactory::save(*this)},
        { "reset", CommandFactory::reset(*this)},
        { "auth", CommandFactory::auth(*this)}
    };

};