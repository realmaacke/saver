#pragma once
#include <filesystem>


namespace fs = std::filesystem;

class Object {
public:
    Object(const fs::path& object_dir);

    std::string file_to_blob(
        const fs::path& path,
        const std::string& prefix,
        const std::ios::openmode mode = std::ios::binary
    );
    void blob_to_file(
        const std::string& hash,
        const std::string& content
    );

    std::string file_permissions(const fs::path& path);

    std::string retrive_stored_blob(const std::string& hash);

private:
    fs::path object_dir;
};