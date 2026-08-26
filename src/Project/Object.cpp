#include "Project/Object.hpp"
#include <filesystem>

namespace fs = std::filesystem;

void Object::set_object_dir(const fs::path& root_dir) {
    this->object_dir = fs::path(root_dir / "/objects/");
}

fs::path Object::get_object_dir() {
    return this->object_dir;
}