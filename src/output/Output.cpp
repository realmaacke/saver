#include "Output/Output.hpp"
#include <iostream>
#include <string>

void Output::error(const std::string &msg) {
    std::cout << "[Saver]: " << msg.c_str() << std::endl;
}

void Output::print(const std::string &msg) {
    std::cout << "[Saver]: " << msg.c_str() << std::endl;
}

void Output::multiple_errors(std::vector<std::string> &messages) {
    for (std::string& msg : messages) {
        std::cout << "[Saver]: " << msg.c_str() << std::endl;
    }
}