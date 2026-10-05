#pragma once
#include <string>
#include <nlohmann/json.hpp>

struct UserInfo {
    std::string username;
    nlohmann::json token;
    int user_id;
};

class User {
public:
    void setConnected(bool value);
    bool isConnected();

    int connectUser();

    int disconnectUser();

    UserInfo getUser();

private:
    bool is_connected = false;
};