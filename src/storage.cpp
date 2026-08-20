#include <cstdlib>
#include "storage.hpp"
#include <filesystem>
#include <iostream>
#include <string>

fs::path Storage::getConfigDirectory() {
    char* location = nullptr;
    constexpr const char* folderName = "saver";

    #if defined(_WIN32)
        location = std::getenv("APPDATA");

        if(location)
            return fs::path(location) / folderName;

    #elif defined( __APPLE__)
        location = std::getenv("HOME");
        if (location)
            return fs::path(location) / "Library" / "Application Support" / folderName;
        
    #elif defined(__linux__)
        location = std::getenv("XDG_DATA_HOME");

        if (location && *location) {
            return fs::path(location) / folderName;
        }
        const char* home = std::getenv("HOME");
        if (home)
            return fs::path(home) / ".local" / "share" / folderName;
    #endif

    throw std::runtime_error("Invalid operating system");
}

int Storage::createConfigDirectory() {
    fs::path directory = this->getConfigDirectory();

    if (fs::exists(directory)) {
        return 0;
    }
    return fs::create_directories(directory);
}