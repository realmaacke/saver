#include <string>
#include <filesystem>
#include "Project/Project.hpp"
#include "Output/Output.hpp"


namespace fs = std::filesystem;

Project::Project() {
    this->has_project = false;
    this->has_root();
}

/**
* Method creates a new Project.
*/
void Project::set_root(const std::string& path) {
    this->root_dir = path;
}

/**
* Called at start to check if in Project.
*/
void Project::has_root() {
    std::string saver_dir = fs::current_path().string() + "/.saver/";
    this->has_project = fs::is_directory(saver_dir);
}