#pragma once

#define ResourceDir RealResourceDir
#include "../../include/Environment.hpp"
#undef ResourceDir

#include <filesystem>
#include <string>

namespace fs = std::filesystem;

inline fs::path& MockResourcePath() {
    static fs::path p;
    return p;
}

inline fs::path ResourceDir(const std::string& /*unused*/) {
    return MockResourcePath();
}