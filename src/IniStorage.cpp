#include "IniStorage.hpp"
#include "Output/Output.hpp"
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <map>
#include <string>

namespace fs = std::filesystem;

IniStorage::IniStorage(const fs::path& path) {
    if (!path.empty()) {
        this->filePath = path;
    }
};

void IniStorage::createStorage() {
    Output::debug("path", this->filePath);
    if (!fs::exists(this->filePath.parent_path())) {
        fs::create_directories(this->filePath.parent_path());
    }

    std::ofstream out(this->filePath);
    out << "" << std::endl;
}

void IniStorage::loadStorage() {
    if (!fs::exists(this->filePath)) {
        Output::error("IniStorage file has not been selected");
        return;
    }

    std::ifstream infile(this->filePath);
    std::string line;

    while (std::getline(infile, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }

        if (line.find("=") == std::string::npos) {
            continue;
        }
        size_t equal_pos = line.find("=");

        std::string key = line.substr(0, equal_pos);
        std::string value = line.substr(equal_pos + 1);
        // deletes ""
        std::erase(value, '"');

        this->values[key] = value;
    }
};

void IniStorage::saveStorage() {
    std::ofstream file(this->filePath, std::ios::trunc);

    if (!file.is_open()) {
        return;
    }

    for (std::map<std::string, std::string>::const_iterator it = this->values.begin();
        it != this->values.end(); it++) {
            file << it->first << "=" << it->second << "\n";
    }
    return;
};

void IniStorage::updateStorage(
    const std::string& key,
    const std::string& value
) {
    this->values[key] = value;
};

std::string IniStorage::getValue(const std::string& key) {
    if (!this->values.contains(key)) {
        Output::error("getValue(), key does not exist");
        return "";
    }
    return this->values[key];
}