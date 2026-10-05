#pragma once
#include <filesystem>
#include <map>
#include <optional>
#include <string>
#include <vector>

namespace fs = std::filesystem;
class INIStorage{
public:
    INIStorage();
    INIStorage(const fs::path& path);
    
    void createStorage();
    void loadStorage();
    void saveStorage();
    void updateStorage(const std::string& key, const std::string& value);
    void updateMultipleValues(
        const std::map<std::string, std::string>& value_map
    );
    std::optional<std::string> getValue(const std::string& key);
private:
    std::optional<fs::path> storage_path;
    std::map<std::string, std::string> values;
};