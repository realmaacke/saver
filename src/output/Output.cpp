#include "Output/Output.hpp"
#include <iostream>
#include <string>

/*
* Method that prints an error.
* TODO: add color support.
*/
void Output::error(const std::string &msg) {
    std::cout << "[Saver]: " << msg.c_str() << std::endl;
}

/*
* Method that prints a message.
* TODO: add color support.
* */
void Output::print(const std::string &msg) {
    std::cout << "[Saver]: " << msg.c_str() << std::endl;
}

/*
* Method that prints multiple errors.
* TODO: add color support.
*/
void Output::multiple_errors(std::vector<std::string> &messages) {
    for (std::string& msg : messages) {
        std::cout << "[Saver]: " << msg.c_str() << std::endl;
    }
}

/*
* Method used for debuging.
* Prints message along with a customizable tag.
* TODO: add color support.
* */
void Output::debug(const std::string &type, const std::string &msg) {
    std::cout << "["+ type +"]" << msg.c_str() << std::endl;
}

void Output::print_progress_bar(curl_off_t sent, curl_off_t total) {
    if (total <= 0) return;
    int percent = static_cast<int>((sent * 100) / total);
    int bar_width = 40;
    int filled = (bar_width * percent) / 100;

    std::cout << "\r[";
    for (int i = 0; i < bar_width; ++i) {
        std::cout << (i < filled ? '#' : ' ');
    }
    std::cout << "] " << percent << "%" << std::flush;

    if (sent >= total) {
        std::cout << "\n";
    }
}