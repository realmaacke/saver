#pragma once

#include "Output/Codes/ErrorCodes.hpp"
#include "Output/Output.hpp"
#include <cstdlib>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

inline fs::path LocalConfigDir() {
    char* location = nullptr;
    constexpr const char* configName = "saver";
    #if defined(_WIN32)
        location = std::getenv("APPDATA");

        if (location)
            return fs::path(location) / configName;

        Output::error(ErrorCode::ENV_NO_DIR, __FUNCTION__);
        return {};

    #elif defined(__APPLE__)
        location = std::getenv("HOME");
        if (location)
        return fs::path(location) / "Library" / "Application Support" / configName;
    
        Output::error(ErrorCode::ENV_NO_DIR, __FUNCTION__);
        return {};

    #elif defined(__linux__)
        location = std::getenv("XDG_DATA_HOME");

        if (location && *location)
            return fs::path(location) / configName;

        const char* home = std::getenv("HOME");

        if (home)
            return fs::path(home) / ".local" / "share" / configName;

        Output::error(ErrorCode::ENV_NO_DIR, __FUNCTION__);
        return {};
    #endif
}

inline fs::path LocalConfigFile() {
    fs::path file = LocalConfigDir() / "global_saver.ini";

    if (!fs::exists(file)) {
        Output::error(ErrorCode::INVALID_PATH, __FUNCTION__);
        return {};
    }

    return file;
}

inline fs::path ResourceDir(const std::string& resouce)  {
    fs::path local_path = fs::path("resources") / resouce;
    if (fs::exists(local_path)) {
        return local_path;
    }

    // Fallback to installed system path passed via Meson (-DAPP_RESOURCE_DIR)
    #ifdef APP_RESOURCE_DIR
    fs::path installed_path = fs::path(APP_RESOURCE_DIR) / resouce;
    if (fs::exists(installed_path)) {
        return installed_path;
    }
    #endif

    throw std::runtime_error("Resource not found");
}