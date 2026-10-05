#include "INIStorage.hpp"
#include "Output/Codes/ErrorCodes.hpp"
#include "Output/Output.hpp"
#include <filesystem>
#include <fstream>
#include <map>
#include <string>

INIStorage::INIStorage() {

}

INIStorage::INIStorage(const fs::path& path) {
    if (!path.empty()) this->storage_path = path;
}

void INIStorage::createStorage() {
    if (this->storage_path.has_value() && this->storage_path->has_parent_path()) {
        fs::create_directories(this->storage_path.value().parent_path());
    } else {
        Output::error(ErrorCode::NO_INI_FILE, __FUNCTION__);
        return;
    }

    std::ofstream out(this->storage_path.value());
    out << "" << std::endl;
}

void INIStorage::loadStorage() {
    if (!this->storage_path.has_value()) {
        Output::error(ErrorCode::NO_INI_FILE, __FUNCTION__);
        return;
    }

    if (!fs::exists(this->storage_path.value())) {
        Output::error(ErrorCode::INVALID_PATH, __FUNCTION__);
        return;
    }

    std::ifstream infile(this->storage_path.value());
    std::string line;

    while (std::getline(infile, line)) {
        if (line.empty() || line[0] == '#') continue;

        if (line.find("=") == std::string::npos) continue;

        size_t eq_pos = line.find("=");
    
        std::string key, value;

        key = line.substr(0, eq_pos);
        value = line.substr(eq_pos + 1);

        std::erase(value, '"');

        this->values[key] = value;
    }
}

void INIStorage::saveStorage() {
    if (!this->storage_path.has_value()) {
        Output::error(ErrorCode::NO_INI_FILE, __FUNCTION__);
        return;
    }

    if (!fs::exists(this->storage_path.value())) {
        Output::error(ErrorCode::INVALID_PATH, __FUNCTION__);
        return;
    }

    std::ofstream file(this->storage_path.value(), std::ios::trunc);

    if (!file.is_open()) {
        Output::error(ErrorCode::FILE_ERROR, __FUNCTION__);
        return;
    }

    for (std::map<std::string, std::string>::const_iterator it = this->values.begin();
        it != this->values.end(); it++) {
        file << it->first << "=" << it->second << "\n";
    }
    return;
}

void INIStorage::updateStorage(
    const std::string& key,
    const std::string& value
) {
    this->values[key] = value;
}

void INIStorage::updateMultipleValues(
    const std::map<std::string, std::string>& value_map
) {
    for (std::map<std::string, std::string>::const_iterator it = value_map.begin();
        it != value_map.end(); it++) {
        this->values[it->first] = it->second;
    }
}

std::optional<std::string> INIStorage::getValue(const std::string& key) {
    if (!this->values.contains(key)) {
        Output::error(ErrorCode::BAD_KEY, __FUNCTION__);
        return std::nullopt;
    }
    return this->values[key];
}