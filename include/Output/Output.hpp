#pragma once
#include <string>
#include <vector>
class Output {
    // TODO:: Add color support
public:
    static void error(const std::string& msg);
    static void print(const std::string& msg);
    static void multiple_errors(std::vector<std::string>& messages);
};