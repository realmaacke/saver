#pragma once
#include <map>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

class IniStorage {
public:
    IniStorage();
    IniStorage(const fs::path& path);
    void createStorage();
    // Loads into values
    void loadStorage();
    void saveStorage();
    void updateStorage(const std::string& key, const std::string& value);
    std::string getValue(const std::string& key);

private:
    fs::path filePath = "";
    std::map<std::string, std::string> values;
};