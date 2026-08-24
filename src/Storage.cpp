#include <cstdlib>
#include "Storage.hpp"
#include <filesystem>
#include <fstream>
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

std::string Storage::getConfigFile() {
    return fs::path(this->getConfigDirectory() / this->configFile).generic_string();
}

// Checks if dirr exists and if config file exists.
// if not create them.
int Storage::createConfigDirectory() {
    fs::path directory = this->getConfigDirectory();

    if (fs::exists(directory)) {
        if (fs::exists(directory / this->configFile)) {
            return 0;
        }
        return this->createConfigFile();
    }
    return fs::create_directories(directory);
}

bool Storage::loadConfig() {
    std::ifstream file(this->getConfigFile());
    if (!file.is_open()) {
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }

        std::size_t eq = line.find('=');
        if (eq == std::string::npos) {
            continue;
        }

        std::string key = line.substr(0, eq);
        std::string value = line.substr(eq + 1);
        this->values[key] = value;
    }

    return true;
}

bool Storage::saveConfig() {
    std::ofstream file(this->getConfigFile(), std::ios::trunc);
    if (!file.is_open()) {
        return false;
    }

    for (std::map<std::string, std::string>::const_iterator it = this->values.begin();
         it != this->values.end(); ++it) {
        file << it->first << "=" << it->second << "\n";
    }

    return true;
}

std::string Storage::getFromConfig(const std::string& key, const std::string& fallback) const {
    std::map<std::string, std::string>::const_iterator it = this->values.find(key);
    if (it == this->values.end()) {
        return fallback;
    }
    return it->second;
}

void Storage::addToConfig(const std::string& key, const std::string& value) {
    this->values[key] = value;
    this->saveConfig();
}

bool Storage::createConfigFile() {
    if (fs::exists(this->getConfigFile())) {
        return true;
    }
    std::ofstream configFile(this->getConfigFile());
    configFile.close();
    return false;
}