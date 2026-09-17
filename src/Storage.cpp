#include <cstdlib>
#include "Storage.hpp"
#include "Output/Output.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>


fs::path Storage::getResourcePath(const std::string& fileName) {
    // Try dev/local path (relative to current working directory)
    fs::path local_path = fs::path("resources") / fileName;
    if (fs::exists(local_path)) {
        return local_path;
    }

    // Fallback to installed system path passed via Meson (-DAPP_RESOURCE_DIR)
    #ifdef APP_RESOURCE_DIR
    fs::path installed_path = fs::path(APP_RESOURCE_DIR) / fileName;
    if (fs::exists(installed_path)) {
        return installed_path;
    }
    #endif

    throw std::runtime_error("Resource not found: " + fileName);
}

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

void Storage::environment_setup() {
    fs::path config_dir = this->getConfigDirectory();

    // Create config directories, safe to re-run.
    fs::create_directories(config_dir);
    // Creates file if it does not exist.
    if (!fs::exists(config_dir / this->configFile)) 
        this->createConfigFile();

    this->loadConfig();
}

bool Storage::createConfigFile() {
    const std::string configPath = this->getConfigFile();
    const std::string basePath = this->getResourcePath("config_base.ini");

    if (fs::exists(configPath)) {
        return true;
    }

    try {
        fs::copy_file(basePath, configPath);
        return true;
    } catch (const fs::filesystem_error& e) {
        Output::error(e.what(), __FUNCTION__);
        return false;
    }
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