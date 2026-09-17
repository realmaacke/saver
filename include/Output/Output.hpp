#pragma once
#include "curl/system.h"
#include <string>
#include <vector>
class Output {
    // TODO:: Add color support
public:
    static void debug(const std::string& type, const std::string& msg);
    static void error(const std::string& msg);
    static void print(const std::string& msg);
    static void multiple_errors(std::vector<std::string>& messages);

    static void print_progress_bar(curl_off_t sent, curl_off_t total);
};