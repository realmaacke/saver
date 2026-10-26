#pragma once
#include <nlohmann/json.hpp>
#include <string>

struct ProjectCreation {
    struct Request {

    };

    struct Response {
        nlohmann::json data;
        bool success;
        std::string message;

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(Response, data, success, message)
    };
};

struct ProjectGetLatestHead {
    struct Request {

    };
    struct Response {
        nlohmann::json data;
        bool success;
        std::string message;

        NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Response, data, success, message)
    };
};