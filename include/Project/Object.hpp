#pragma once
#include <filesystem>

namespace fs = std::filesystem;

class Object {
public:
    void create_obj_directory(const fs::path& root_path);
    // Create directory.
    // Append files to the object/
    // return files from object/
private:
    std::string obj_dir = "";
};