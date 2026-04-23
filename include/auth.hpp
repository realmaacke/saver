#pragma once
#include <filesystem>

#include "sender.hpp"

namespace fs = std::filesystem;


class Auth {
    public:
        Auth() = default;

        int store_key(fs::path ssh_path);

        int use_key(Sender& sender);
        int upload_key(Sender& sender);

        int login_user(Sender& sender);
        int store_user();
};