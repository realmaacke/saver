#pragma once
#include <filesystem>
#include <map>
#include <string>

namespace fs = std::filesystem;

class Storage {
public:
    int createConfigDirectory();
    bool createConfigFile();

    bool loadConfig();
    bool saveConfig();

    std::string getFromConfig(const std::string& key, const std::string& fallback = "") const;
    void addToConfig(const std::string& key, const std::string& value);
private:
    // config dirr
    fs::path getConfigDirectory();
    std::string getConfigFile();

    // config file
    std::string configFile = "saver_conf.ini";
    std::map<std::string, std::string> values;
};