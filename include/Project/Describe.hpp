#pragma once

#include <chrono>
#include <optional>
#include <string>
 
struct Describe {
    std::string tree_hash;
    std::optional<std::string> parent_hash; // empty for the first commit
    std::string message;
    std::chrono::system_clock::time_point timestamp;
 
    // "<tree_hash>\n<parent_hash|->\n<unix_seconds>\n\n<message>\n"
    std::string serialize() const;
};
