#include <iostream>
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


std::string create_project_get_name(std::string path) {
    std::string name;
    if (path.empty()) {
        std::cout << "Project name: ";
        std::getline(std::cin, name);
    } else {
        std::string selection;
        std::string dirName = path.substr(path.find_last_of("/") + 1);

        std::cout << "Use directory name (" << dirName << ") as Project name [Y/n]: ";
        std::getline(std::cin, selection);

        if (selection == "n" || selection == "N") {
            std::cout << "Project name: ";
            std::getline(std::cin, name);
        } else {
            name = dirName;
        }
    }

    if (name.empty()) {
        create_project_get_name(path);
    }

    return name;
}

void Project::create_project(std::string path) {
    std::string name;
    std::string chapter;

    std::cout << "Creating a saver project." << std::endl;
    name = create_project_get_name(path);

    std::cout << "Name of Chapter (branch): ";
    std::getline(std::cin, chapter);

    this->createProjectFiles();
}
/**
* Method that adds files to cache.
* Called from Command::add();
*/
bool Project::populate_cache(const std::string& path) {
    if (!fs::is_regular_file(this->root_dir + path)
        || !fs::is_directory(this->root_dir + path))
    {
        Output::error("Path is not valid.");
        return 1;
    }

    return 0;
}

/**
* Method that adds files to cache.
*/
bool Project::reset_cache() {
    return 0;
}

void Project::createProjectFiles() {
    // create .saver dir.
    // create .saver/info.ini ? or json
    // create chache file
}