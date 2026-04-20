#pragma once

#include <string>

enum class os_enum {
    windows,
    linux,
    mac,
    unknown,
};

// function declarations
os_enum getOS();
std::string getConfigPath();