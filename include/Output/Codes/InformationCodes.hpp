#pragma once
#include <string_view>

enum class InfoCode {
    INVALID_COMMAND,
    UNKNOWN_COMMAND,
    PROJECT_NOT_FOUND,
    ALREADY_IN_PROJECT,
    NO_PATH_SPECIFIED,

    // Specific command codes
    NO_DESCRIBE_WITH_COMMAND,
    INDEX_NO_DEL,

    // Upload
    NO_DESCRIBE_FOUND,
    NO_CHAPTER_FOUND,

    // Auth
    NOT_LOGGED_IN,
    ALREADY_LOGGED_IN,
    COULD_NOT_LOG_IN,
    SUCCESSFULY_LOGGED_IN,
    SUCCESFULY_LOGGED_OUT,

};

struct InfoEntry {
    InfoCode type;
    std::string_view message;
};

constexpr InfoEntry info_codes[] = {
    {
        .type = InfoCode::INVALID_COMMAND,
        .message = "Invalid Syntax"
    },
    {
        .type = InfoCode::UNKNOWN_COMMAND,
        .message = "Unknown Command"
    },
    {
        .type = InfoCode::PROJECT_NOT_FOUND,
        .message = "Not inside a saver project"
    },
    {
        .type = InfoCode::ALREADY_IN_PROJECT,
        .message = "Already inside a saver project"
    },
    {
        .type = InfoCode::NO_PATH_SPECIFIED,
        .message = "Path is not specified"
    },
    {
        .type = InfoCode::NO_DESCRIBE_WITH_COMMAND,
        .message = "Syntax: saver \"message \""
    },
    {
        .type = InfoCode::INDEX_NO_DEL,
        .message = "Could not remove file from index"
    },
    {
        .type = InfoCode::NO_DESCRIBE_FOUND,
        .message = "You need to describe your changes first"
    }, {
        .type = InfoCode::NO_CHAPTER_FOUND,
        .message = "Could not determine current chapter"
    },
    {
        .type = InfoCode::NOT_LOGGED_IN,
        .message = "You need to connect with the remote first"
    },
    {
        .type = InfoCode::ALREADY_IN_PROJECT,
        .message = "You already are logged in"
    },
    {
        .type = InfoCode::COULD_NOT_LOG_IN,
        .message = "Invalid credentials"
    },
    {
        .type = InfoCode::SUCCESSFULY_LOGGED_IN,
        .message = "Succesfully connected to the remote"
    }, {
        .type = InfoCode::SUCCESFULY_LOGGED_OUT,
        .message = "Successfully dissconnected from the remote"
    }
};

constexpr std::string_view to_message(InfoCode code) {
    for (const InfoEntry& entry : info_codes) {
        if (entry.type == code) return entry.message;
    }
    return "Unknown info code occurred.";
};