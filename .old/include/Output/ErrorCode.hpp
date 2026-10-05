#pragma once
#include <string_view>

// All different type of errors
enum class ErrorType {
    // User specific
    AUTH_REQUIRED,
    USER_RETRIVAL,
    COULD_NOT_AUTH,

    // Project specific
    NO_DESCRIBE,
    DETERMINE_CHAPTER,
    COULD_NOT_CREATE_PROJ,
    EMPTY_PROJECT,
    ALREADY_IN_PROJECT,


    // Command
    INVALID_USE_OF_COMMAND,

    // Directory
    INVALID_DIRECTORY,
    DIRECTORY_NOT_EMPTY,

    // File
    FILE_CANT_OPEN,
    FILE_CANT_BE_CREATED,
    FILE_CANT_BE_COPIED,

    // PATH
    INVALID_PATH,
    NO_PATH_SPECIFIED,
    
    // Internal
    SEPARATOR,
    BAD_KEY,

    // Fetching
    NON_JSON_REQUEST,
    BAD_UPLOAD,
};

constexpr std::string_view error_to_string(ErrorType err) {
    switch (err) {
        case ErrorType::ALREADY_IN_PROJECT:
            return "Already inside a project";
        case ErrorType::COULD_NOT_AUTH:
            return "Could not authenticate with the remote";
        case ErrorType::FILE_CANT_BE_COPIED:
            return "File cant be copied";
        case ErrorType::BAD_KEY:
            return "Key does not exist";
        case ErrorType::AUTH_REQUIRED:
            return "You must be logged in";
        case ErrorType::USER_RETRIVAL:
            return "Could not retrive the current user";
        case ErrorType::DETERMINE_CHAPTER:
            return "Could not determine current chapter";
        case ErrorType::NO_DESCRIBE:
            return "Describe the changes";
        case ErrorType::COULD_NOT_CREATE_PROJ:
            return "Could not create project";
        case ErrorType::EMPTY_PROJECT:
            return "Project is empty";
        case ErrorType::INVALID_USE_OF_COMMAND:
            return "Invalid use of command";
        case ErrorType::INVALID_DIRECTORY:
            return "Not in a valid directory";
        case ErrorType::DIRECTORY_NOT_EMPTY:
            return "Directory is not empty";
        case ErrorType::INVALID_PATH:
            return "Invalid Path";
        case ErrorType::NO_PATH_SPECIFIED:
            return "Path is not specified";
        case ErrorType::FILE_CANT_OPEN:
            return "Cant open file";
        case ErrorType::FILE_CANT_BE_CREATED:
            return "Failed to create file";
        case ErrorType::SEPARATOR:
            return "Separator not found";
        case ErrorType::NON_JSON_REQUEST:
            return "Server returned a non-JSON response";
        case ErrorType::BAD_UPLOAD:
            return "Could not upload to server";
    }
    return "Unknown error occurred.";
}