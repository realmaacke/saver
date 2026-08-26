#pragma once
#include <string>
#include <map>
#include <fstream>
#include <sstream>
#include <algorithm>

class Ini {
public:
    using Section = std::map<std::string, std::string>;

    void load(const std::string& path) {
        std::ifstream file(path);
        std::string line, section;
        while (std::getline(file, line)) {
            trim(line);
            if (line.empty() || line[0] == ';' || line[0] == '#') continue;

            if (line.front() == '[' && line.back() == ']') {
                section = line.substr(1, line.size() - 2);
                continue;
            }

            auto eq = line.find('=');
            if (eq == std::string::npos) continue;

            std::string key = line.substr(0, eq);
            std::string val = line.substr(eq + 1);
            trim(key);
            trim(val);
            data[section][key] = val;
        }
    }

    void save(const std::string& path) const {
        std::ofstream file(path);
        for (const auto& [section, kv] : data) {
            if (!section.empty()) file << '[' << section << "]\n";
            for (const auto& [key, val] : kv)
                file << key << " = " << val << '\n';
            file << '\n';
        }
    }

    std::string get(const std::string& section, const std::string& key,
                     const std::string& fallback = "") const {
        auto sIt = data.find(section);
        if (sIt == data.end()) return fallback;
        auto kIt = sIt->second.find(key);
        return kIt != sIt->second.end() ? kIt->second : fallback;
    }

    void set(const std::string& section, const std::string& key, const std::string& value) {
        data[section][key] = value;
    }

private:
    std::map<std::string, Section> data;

    static void trim(std::string& s) {
        auto notSpace = [](unsigned char c) { return !std::isspace(c); };
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), notSpace));
        s.erase(std::find_if(s.rbegin(), s.rend(), notSpace).base(), s.end());
    }
};