#pragma once
#include <string_view>

enum class ErrorCode {
    // auth error
    AUTH_REQUIRED,
    AUTH_FAILED_INTERNAL,

    // fetch errors
    NON_JSON_REQUEST,
    // file error
    INVALID_PATH,
    NO_INI_FILE,
    FILE_ERROR,
    FILE_MALFORMED,

    // Arrays
    BAD_KEY,

    ENV_NO_DIR,
    INTERNAL_RESOURCE_MISSING,
    INTERNAL_FILE_ERROR,

    INTERNAL_VARS_UNSET,
};

struct ErrorEntry {
    ErrorCode type;
    std::string_view message;
};

constexpr ErrorEntry error_codes[] = {
{ .type = ErrorCode::AUTH_REQUIRED, .message = "Authentication required." },
{ .type = ErrorCode::NO_INI_FILE, .message = "No ini file selected."},
{ .type = ErrorCode::INVALID_PATH, .message = "Path is not valid"},
{ .type = ErrorCode::FILE_ERROR, .message = "File access, deined"},
{ .type = ErrorCode::BAD_KEY, .message = "Invalid key"},
{ .type = ErrorCode::NON_JSON_REQUEST, .message = "Response is not in JSON"},
{ .type = ErrorCode::ENV_NO_DIR, .message = "Could not found config dir on your system"},
{ .type = ErrorCode::INTERNAL_RESOURCE_MISSING, .message = "Resource is missing, reinstall saver"},
{ .type = ErrorCode::INTERNAL_FILE_ERROR, .message = "Internal file error, try reinstalling saver"},
{ .type = ErrorCode::INTERNAL_VARS_UNSET, .message = "Internal error, variables unset, reinstall saver"},
{.type = ErrorCode::FILE_MALFORMED, .message = "File is malformed."},
{.type = ErrorCode::AUTH_FAILED_INTERNAL, .message = "Authentication failed on the remote"}
};

constexpr std::string_view to_message(ErrorCode code) {
    for (const ErrorEntry& entry : error_codes) {
        if (entry.type == code) return entry.message;
    }
    return "Unknown error occurred.";
};