#include "Project/Object.hpp"
#include "Output/Output.hpp"
#include <filesystem>

namespace fs = std::filesystem;

void Object::create_obj_directory(const fs::path& root_dir) {
    if (root_dir.empty()) {
        Output::error("Root dir is not set.");
        return;
    }

    this->obj_dir = fs::path(root_dir / "/objects/");

    if (!fs::exists(this->obj_dir)) {
        fs::create_directories(this->obj_dir);
    }
};
