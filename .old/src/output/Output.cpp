#include "Output/Output.hpp"
#include "Output/ErrorCode.hpp"
#include <iostream>
#include <string>

namespace Color {
    constexpr std::string_view Reset   = "\033[0m";
    constexpr std::string_view Red     = "\033[31m";
    constexpr std::string_view Green   = "\033[32m";
    constexpr std::string_view Yellow  = "\033[33m";
    constexpr std::string_view Blue    = "\033[34m";
    constexpr std::string_view Bold    = "\033[1m";
}

void Output::warning(const ErrorType &type) {
    std::cout << error_to_string(type) << std::endl;
}

/*
* Method that prints an error.
* TODO: add color support.
*/
void Output::error(const ErrorType& type, const std::string& function) {
    std::cout
        << "[" 
        << Color::Red
            <<"Error"
        << Color::Reset 
        << "]: "
        << Color::Red
        << "source"
        << function << "()"
        << " reason:"
        << error_to_string(type) << Color::Reset << std::endl;
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