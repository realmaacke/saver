#pragma once
#include <nlohmann/json.hpp>
#include <string>

struct ConnectUserDTO {
    struct Request {
        std::string username;
        std::string password;
        NLOHMANN_DEFINE_TYPE_INTRUSIVE(Request, username, password)
    };
    struct Response {
        nlohmann::json data;
        bool success;
        std::string message;
        NLOHMANN_DEFINE_TYPE_INTRUSIVE(Response, data, success, message)
    };
};

struct UserVerify {
    struct Response {
        nlohmann::json data;
        bool success;
        std::string message;
        NLOHMANN_DEFINE_TYPE_INTRUSIVE(Response, data, success, message)
    };
};

struct RetriveUser {
    struct Request {

    };
	struct Response {
        nlohmann::json data;
        bool success;
        std::string message;
        NLOHMANN_DEFINE_TYPE_INTRUSIVE(Response, data, success, message)
	};
};