#pragma once
#include <string_view>

// All different type of errors
enum class ErrorType {
    // User specific
    
    AuthRequired,

    // Project specific


    // General


    // Internal
};

constexpr std::string_view error_to_string(ErrorType err) {
    switch (err) {
        case ErrorType::AuthRequired: return "Test";
    }
    return "Unknown error occurred.";
}