#include "utils.hpp"
#include <cstdlib>

#if defined(__linux__) || defined(__APPLE__) || defined(__unix__) || defined(_WIN32)
#include <filesystem>
namespace fs = std::filesystem;
#endif

os_enum getOS() {
    #if defined(_WIN32) || defined(_WIN64)
        return os_enum::windows;
    #elif defined(__APPLE__) || defined(__MACH__)
        return os_enum::mac;
    #elif defined(__linux__)
        return os_enum::linux;
    #elif defined(__unix__)
        return os_enum::linux;
    #else
        return os_enum::unknown;
    #endif
}

std::string getConfigPath() {
    #if defined(_WIN32)
        const char* base = std::getenv("APPDATA");
        return std::string(base ? base : "") + "\\saver\\";

    #elif defined(__APPLE__)
        const char* home = std::getenv("HOME");
        return std::string(home ? home : "") + "/Library/Application Support/saver/";

    #elif defined(__linux__)
        const char* config = std::getenv("XDG_CONFIG_HOME");
        if (config)
            return std::string(config) + "/saver/";

        const char* home = std::getenv("HOME");
        return std::string(home ? home : "") + "/.config/saver/";

    #else
        return "./";
    #endif
}