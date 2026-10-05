#pragma once
#include <stdio.h>
#include <string>
#include <optional>
#include <iostream>
#include <string_view>


// For hidden prompt
#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

inline std::string prompt(
    std::string_view label,
    std::optional<std::string> default_value = std::nullopt
) {
    if (default_value) {
        std::cout << label << " (" << *default_value << "): ";
    } else {
        std::cout << label << ": ";
    }

    std::string input;
    std::getline(std::cin, input);

    if (input.empty() && default_value) {
        return *default_value;
    }
    return input;
}

inline bool polar_prompt(std::string_view label, bool default_value = false) {
    std::cout << label << " " << (default_value ? "[Y/n]" : "[y/N]") << ": ";
    
    std::string input;
    std::getline(std::cin, input);

    if (input.empty()) return default_value;
    return input[0] == 'y' || input[0] == 'Y';
}

inline std::string hidden_prompt(std::string_view label, bool useAsterisk = false) {
    std::string res;
    std::cout << label << ": ";

#ifdef _WIN32
    char ch;
    while((ch = _getch()) != '\r') {
        if (ch == '\b') {
            if (!res.empty()) {
                std::cout << "\b \b";
                res.pop_back();
            }
        } else {
            res += ch;
            if (useAsterisk) {
                std::cout << '*';
            }
        }
    }
#else
    termios oldt;
    tcgetattr(STDIN_FILENO, &oldt);
    termios newt = oldt;
    newt.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    char ch;

    while((ch = getchar()) != '\n') {
         if (ch == 127) { // delete key
            if (!res.empty()) {
                std::cout << "\b \b"; // erase previous char
                res.pop_back();
            }
         } else {
            res += ch;
            if (useAsterisk) {
                std::cout << '*' << std::flush;
            }
         }
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
#endif

    std::cout << "\n";
    return res;
};