#include <chrono>
#include <iostream>
#include <optional>
#include <string>
#include <filesystem>
#include "Project/Project.hpp"
#include "Output/Output.hpp"
#include "Project/Cache.hpp"
#include "Project/Describe.hpp"
#include "Project/Tree.hpp"


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
    this->cache_.add(path);
    return false;
}

/**
* Method that adds files to cache.
*/
bool Project::reset_cache() {
    this->cache_.clear();
    return true;
}

bool Project::remove_from_cache(const std::string& path) {
    Output::print("Removed file: " + path);
    this->cache_.remove(path);
    return true;
}

void Project::createProjectFiles() {
    // create .saver dir.
    // create .saver/info.ini ? or json
    // create chache file
}

bool Project::describe_cache(std::string& message) {
    std::string branch = this->head_.branch();

    Tree tree;

    for (const CacheEntry& entry : this->cache_.entries()) {
        std::string hash = this->cacheStore_.store(entry.cached_path);
        tree.add(entry.original_path, hash);
    }

    std::string tree_hash = this->cacheStore_.store_bytes(tree.serialize());

    std::optional<std::string> parent = this->refStore_.get(branch);

    // Creating the describe.
    Describe desc;

    desc.tree_hash = tree_hash;
    desc.parent_hash = parent;
    desc.message = message;
    desc.timestamp = std::chrono::system_clock::now();
    std::string describe_hash = this->cacheStore_.store_bytes(desc.serialize());

    this->refStore_.set(branch, describe_hash);
    this->cache_.clear();

    Output::print("Described: " + describe_hash);
    return true;
}