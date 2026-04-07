#include "repo.hpp"
#include <filesystem>
#include <iostream>


namespace fs = std::filesystem;

bool checkExistance(fs::path path) {
    if (fs::exists(path)) {
        return true;
    }
    return false;
}

bool Repository::init() { 
    if (checkExistance(this->objects_path) || checkExistance(this->refs_path)) {
        return false;
    }

    fs::create_directories(this->objects_path);
    fs::create_directories(this->refs_path);

    return checkExistance(this->objects_path) && checkExistance(this->refs_path);
}

bool Repository::add(fs::path path_to_files) {
    fs::path output_path = this->base_path  / path_to_files;
    
    if (!fs::exists(path_to_files)) {
        std::cerr << output_path << " does not exist" << std::endl;
        return false;
    }
    

    if (fs::is_regular_file(path_to_files)) {
        std::cout << "Added file: " << output_path << std::endl;
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

void Repository::print_add_storage() const {
    for (const auto& entry : add_storage) {
        std::cout << "Added item: " << entry << '\n';
    }
}