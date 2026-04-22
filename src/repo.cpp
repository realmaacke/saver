#include "repo.hpp"
#include <filesystem>
#include <iostream>
#include <fstream>

namespace fs = std::filesystem;


void Repository::init(fs::path target_directory) {
    this->target_directory = target_directory;
    this->saver_path = target_directory / ".saver";
    this->index_path = this->saver_path / "index";
    this->saver_ignore = target_directory / ".saverIgnore";
    return;
};

void Repository::initRepo() {
    // Creates folder
    if (!fs::exists(this->saver_path)) {
        fs::create_directories(this->saver_path);
    }
    
    // Stream creates the index file
    if (!fs::exists(this->index_path)) {
        std::ofstream(this->index_path);
    }

    return;
}

bool Repository::status() {
    std::vector<fs::path> commited_files = this->loadFromIndex();

    for (const auto& commit : commited_files) {
        std::cout << "File: " << commit << std::endl;
    }
    return true;
}


/**
 * Recursive method that adds items to index.
 */
bool Repository::add(fs::path path_to_add) {
    fs::path output_path = this->target_directory / path_to_add;

    // if the path itself is ignored, do nothing.
    if (this->executeIgnore(path_to_add)) {
        return true;
    }

    if (!fs::exists(path_to_add)) {
        std::cout << "saver: " << output_path << "did not match any files" << std::endl;
        return false;
    }

    if (fs::is_regular_file(path_to_add)) {
        return this->addToIndex(path_to_add);
    }

    if (fs::is_directory(path_to_add)) {
        for (const auto& entry : fs::directory_iterator(path_to_add)) {
            if(!this->add(entry.path())) {
                return false;
            }
        }
        return true;
    }
    return true;
};

bool Repository::describe() {
    /**
     * Add the commit stuff here.
     */

     return true;
};

bool Repository::save() {
    if (!this->hasCommited) {
        std::cout << "All is saved, try to describe if you got a new save!" << std::endl;
        return true;
    }
    return true;
};

bool Repository::reset() {
    std::ofstream out(this->index_path);

    if (!out) {
        std::cerr << "fatal: saver could not reset the changes" << std::endl;
        return false;
    }

    out << "";
    return true;
};

bool Repository::addToIndex(fs::path path_to_store) {
    std::ofstream out(this->index_path, std::ios::app);

    
    if (!out) {
        std::cerr << "fatal: saver could not store the changes" << std::endl;
        return false;
    }
    
    // Prevents duplicates
    std::vector<fs::path> currentIndex = this->loadFromIndex();

    bool found = false;
    
    for (const auto& path_to_saved : currentIndex) {
        if (path_to_saved == path_to_store) {
            found = true;
        }
    }

    if (!found) {
        out << path_to_store.generic_string() << '\n';
    }
    
    return true;
};

/**
 * method that loads the stored changes and returns them.
 */
std::vector<fs::path> Repository::loadFromIndex() {
    std::ifstream in(this->index_path);
    std::vector<fs::path> stored_changes;

    if (!in) {
        std::cerr << "fatal: saver could not load the changes" << std::endl;
        return stored_changes;
    }

    std::string line;

    while(std::getline(in, line)) {
        if (!line.empty()) {
            stored_changes.emplace_back(line);
        }
    }

    return stored_changes;
};

/**
 * method that reads the ignore files and appends to an array.
 * also returns the array :)
 */
std::vector<fs::path> Repository::readIgnore() const {
    std::vector<fs::path> ignore_rules;

    if (!fs::exists(this->saver_ignore)) return ignore_rules;
    
    std::ifstream file(this->saver_ignore);
    
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) {
            continue;
        }
        ignore_rules.emplace_back(line);
    }

    return ignore_rules;
};

/**
 * Function that gets all the rules in array.
 * Itterates that array and check if the path matches the ignored one.
 */
bool Repository::executeIgnore(fs::path ignored_path) {
    fs::path sanitizedPath = fs::absolute(ignored_path).lexically_normal();

    std::vector<fs::path> ignore_rules = readIgnore();

    for (const auto& rule : ignore_rules) {
        fs::path sanitizedRule = fs::absolute(this->target_directory / rule).lexically_normal();

        auto missMatch = std::mismatch(
            sanitizedRule.begin(), sanitizedRule.end(),
            sanitizedPath.begin(), sanitizedPath.end()
        );

        if (missMatch.first == sanitizedRule.end()) {
            return true;
        }
    }
    return false;
}