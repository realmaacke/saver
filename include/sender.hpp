#pragma once
#include <filesystem>
#include <vector>
#include <curl/curl.h>

namespace fs = std::filesystem;

class Sender {
public:
    Sender() = default;

    int sendFile(
        const std::string& remote,
        const fs::path& base_dir,
        const fs::path& file
    );

    int sendMultipleFiles(
        const std::string& remote,
        const fs::path& base_dir,
        const std::vector<fs::path>& files
    );
private:
};