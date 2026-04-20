#include "auth.hpp"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>

#include "utils.hpp"

namespace fs = std::filesystem;

/**
 * Creates config and its directory if it dosent exists.
 */
void CreateLocalStorage(std::string config_path, std::string ssh_path) {
    std::string config_file = "keys.saver";
    std::string file_content = "ssh_path=" + ssh_path;

    if (!fs::exists(config_path)) {
        fs::create_directories(config_path);
        std::cout << "[Remove] Created Config at: " << config_path << std::endl;
    }

    if (!fs::exists(config_path + config_file)) {
        std::ofstream outfile (config_path + config_file);

        outfile << file_content << std::endl;

        outfile.close();
    } else {
        std::fstream read_file;
        read_file.open(config_path + config_file);
        
        if (read_file.fail()) {
            std::cerr << "Read file failed" << std::endl;
            return;
        }
        
        std::vector<std::string> lines;
        std::string line;
        
        while (getline(read_file, line)) {
            lines.push_back(line);
        }
        
        read_file.close();
        
        std::ofstream write_file (config_path + config_file);
        bool found = false;

        for (const auto& l : lines) {
            if(l.starts_with("ssh_path=")) {
                write_file << file_content << std::endl;
                found = true;
            } else {
                write_file << l << std::endl;
            }
        }

        if (!found) {
            write_file << file_content << std::endl;
        }

        write_file.close();
    }
}


void Auth::store_key(fs::path ssh_path) {

    CreateLocalStorage(getConfigPath(), ssh_path);
}