#include "auth.hpp"
#include <iostream>
#include <filesystem>
#include <fstream>

#include "utils.hpp"

namespace fs = std::filesystem;


void CreateLocalStorage(std::string path) {
    std::cout << "Path is: " << path << std::endl;

    if (!fs::exists(path)) {
        fs::create_directories(path);
        std::cout << "[Remove] Created Config at: " << path << std::endl;

        // TODO::
        // CREATE auth_key FILE
        // READ FILE NAME OF EXISTING SSH KEY IN .ssh
        // IF ALREADY EXISTS, DO A PROMPT (OVERRIDE?)
    }
}


void Auth::store_key(fs::path ssh_path) {
    std::cout << "Path is : " << ssh_path << std::endl;

    CreateLocalStorage(getConfigPath());
}