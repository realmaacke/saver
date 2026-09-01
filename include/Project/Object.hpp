#pragma once
#include <filesystem>
#include <optional>

namespace fs = std::filesystem;

class Object {
    public:
    // Create directory.
    void create_obj_directory(const fs::path& root_path);
    
    // hash the file.
    std::string sha256(const std::string& data);
    std::string transform_file(const fs::path& path);

    // finds the object
    std::optional<fs::path> object_path(const std::string& hash);
    void store_object(const std::string& hash, const std::string& content);
    // Append files to the object/
    // return files from object/
private:
    std::string obj_dir = "";
};