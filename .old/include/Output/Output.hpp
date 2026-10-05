#pragma once
#include "Output/ErrorCode.hpp"
#include "curl/system.h"
#include <string>
#include <vector>
class Output {
public:
    static void error(const ErrorType& type, const std::string& function=__FUNCTION__);
    static void warning(const ErrorType& type);
    static void print(const std::string& message);

    static void debug(const std::string& type, const std::string& msg);

    // This method is redundant, remove it.
    static void multiple_errors(std::vector<std::string>& messages);

    // Curl progress bar.
    static void print_progress_bar(curl_off_t sent, curl_off_t total);
};