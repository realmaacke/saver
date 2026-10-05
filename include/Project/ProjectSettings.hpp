#pragma once
#include <string>
#include <optional>
#include <chrono>

struct ProjectSettings {
    std::string project_name;
    std::string project_remote;
    std::string default_chapter;
    std::optional<std::string> description;
    bool is_private = false;
    std::chrono::system_clock::time_point start_date;
    std::chrono::system_clock::time_point last_sync;
};