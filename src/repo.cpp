#include "repo.hpp"
#include <filesystem>
#include <iostream>
#include <fstream>

namespace fs = std::filesystem;

/**
 * Kind of redundant function.
 * Nice to have :)
 */
bool checkExistance(fs::path path) {
    if (fs::exists(path)) {
        return true;
    }
    return false;
}

/**
 * Redundant function. :)
 */
bool hasIgnoreFile(fs::path saverIgnore) {
    return fs::exists(saverIgnore);
}

/**
 * Function that reads file and returns an array of the content.
 */
std::vector<fs::path> readRules(fs::path base_path) {
    fs::path saver_ignore = base_path / ".saverIgnore";
    std::vector<fs::path> ignore_rules;

    std::ifstream file(saver_ignore);
    std::string line;

    // Return empty
    if (!hasIgnoreFile(saver_ignore)) return ignore_rules;

    // Append all rules to array
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        ignore_rules.emplace_back(line);
    }

    return ignore_rules;
}

/**
 * Method that iterates the ignore rules from a file and enforces them.
 */
bool Repository::ignore(fs::path path_to_files) {
    fs::path absolute_file = fs::absolute(path_to_files).lexically_normal();

    std::vector<fs::path> ignore_rules = readRules(this->base_path);

    for (const auto& rule : ignore_rules) {
        fs::path absolute_rule = fs::absolute(base_path / rule).lexically_normal();

        auto missmatch = std::mismatch(
            absolute_rule.begin(), absolute_rule.end(),
            absolute_file.begin(), absolute_file.end()
        );

        if (missmatch.first == absolute_rule.end()) {
            return true;
        }
    }
    return false;
}

/**
 * Method that inits the repository.
 * Not done!!
 */
bool Repository::init() { 
    if (checkExistance(this->objects_path) || checkExistance(this->refs_path)) {
        return false;
    }

    fs::create_directories(this->objects_path);
    fs::create_directories(this->refs_path);

    return checkExistance(this->objects_path) && checkExistance(this->refs_path);
}

/**
 * Method that captures files to be added.
 * (Recursive if directory with children exists)
 * @param path_to_files - specified path by user
 * @returns boolean
 */
bool Repository::add(fs::path path_to_files) {
    fs::path output_path = this->base_path  / path_to_files;
    fs::path saver_ignore = this->base_path / ".saverIgnore";

    if (this->ignore(path_to_files)) {
        return true;
    }
    
    if (!fs::exists(path_to_files)) {
        std::cerr << output_path << " does not exist" << std::endl;
        return false;
    }

    if (fs::is_regular_file(path_to_files)) {
        std::cout << "Added file: " << output_path.lexically_normal() << std::endl;
        this->add_storage.push_back(path_to_files);
        return true;
    }

    if (fs::is_directory(path_to_files)) {
        
        for (const auto& entry : fs::directory_iterator(path_to_files)) {
            if (!this->add(entry.path())) {
                return false;
            }
        }
        return true;
    }
    return true;
}

/**
 * Temporarly for debugging.
 */
void Repository::print_add_storage() const {
    for (const auto& entry : add_storage) {
        std::cout << "Added item: " << entry << '\n';
    }
}