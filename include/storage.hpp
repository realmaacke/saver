#pragma once
#include <filesystem>

namespace fs = std::filesystem;

class Storage {
public:

private:
    fs::path getConfigDirectory();
    int createConfigDirectory();
    
};