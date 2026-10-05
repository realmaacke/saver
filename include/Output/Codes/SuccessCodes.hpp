#pragma once
#include <string_view>

enum class SuccessCode {
    USER_CONNECTED,
    PROJECT_CREATED,
    SUCCESFULLY_WIPED_INDEX
};

struct SuccessEntry {
    SuccessCode type;
    std::string_view message;
};

constexpr SuccessEntry success_codes[] = {
    {
        .type = SuccessCode::USER_CONNECTED,
        .message = "User succesfully connected."
    },
    {
        .type = SuccessCode::PROJECT_CREATED,
        .message = "Project succesfully created"
    },
    {
        .type = SuccessCode::SUCCESFULLY_WIPED_INDEX,
        .message = "Removed all files from index."
    }
};

constexpr std::string_view to_message(SuccessCode code) {
    for (const SuccessEntry& entry : success_codes) {
        if (entry.type == code) return entry.message;
    }
    return "Unknown code occurred.";
};