#pragma once
#include <filesystem>

#include "sender.hpp"

namespace fs = std::filesystem;


class Auth {
    public:
        Auth() = default;

        void store_key(fs::path ssh_path);

        void use_key(Sender& sender);
        void upload_key(Sender& sender);
    private:
};