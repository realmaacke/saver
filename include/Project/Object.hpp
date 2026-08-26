#pragma once
#include <filesystem>

namespace fs = std::filesystem;

class Object {
public:
    void set_object_dir(const fs::path& root_dir);
    fs::path get_object_dir();
private:
    fs::path object_dir;
};