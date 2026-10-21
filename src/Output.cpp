#include "Output/Output.hpp"
#include "Output/Codes.hpp"
#include <iostream>

namespace Color {
    constexpr std::string_view Reset   = "\033[0m";
    constexpr std::string_view Red     = "\033[31m";
    // constexpr std::string_view Green   = "\033[32m";
    // constexpr std::string_view Yellow  = "\033[33m";
    // constexpr std::string_view Blue    = "\033[34m";
    // constexpr std::string_view Bold    = "\033[1m";
}

void Output::error(const ErrorCode &type, const std::string& function) {
    std::cout
    << "[" 
    << Color::Red
        <<"Error"
    << Color::Reset 
    << "]: "
    << function << "()"
    << " reason: "
    << code_to_message(type) << Color::Reset << std::endl;
}

void Output::info(const InfoCode &type, const std::string& extra) {
    std::cout << code_to_message(type) << extra << std::endl;
}

void Output::success(const SuccessCode &type, const std::string& extra) {
    std::cout << code_to_message(type) << extra << std::endl;
}

void Output::print(const std::string &message) {
    std::cout << message << std::endl;
}