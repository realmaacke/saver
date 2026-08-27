#pragma once
#include <nlohmann/json.hpp>
#include <string>

struct CreateProject {
    struct Request {
        nlohmann::json data;

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(
            Request,
            data
        )
    };

    struct Response {
        bool success;
        nlohmann::json data;
        std::string message;

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(
            Response,
            success,
            data,
            message
        )
    };
};

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
