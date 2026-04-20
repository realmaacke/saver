#pragma once
#include <filesystem>


namespace fs = std::filesystem;


class Auth {
    public:
        Auth() = default;

        void store_key(fs::path ssh_path);
    private:
};