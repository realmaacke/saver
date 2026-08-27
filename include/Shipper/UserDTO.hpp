#pragma once
#include <nlohmann/json.hpp>
#include <string>

struct GetUserInfo {
    struct Response {
        int userId;
        std::string username;
        bool success;
        std::string message;
        
        NLOHMANN_DEFINE_TYPE_INTRUSIVE(
            Response,
            userId,
            username,
            success,
            message
        )
    };
};

struct RenewToken {
    struct Response {
        nlohmann::json token;
        bool success;
        std::string message;

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(
            Response,
            token,
            success,
            message
        )
    };
};

struct ConnectUser {
    struct Request {
        std::string username;
        std::string password;

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(
            Request,
            username,
            password
        )
    };

    struct Response {
        nlohmann::json token;
        bool success;
        std::string message;

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(
            Response,
            token,
            success,
            message
        )
    };
};