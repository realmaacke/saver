#pragma once
#include <nlohmann/json.hpp>
#include <string>

struct ProjectCreation {
    struct Request {

    };

    struct Response {
        bool success;
        nlohmann::json data;
        std::string message;

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(Response, success, data, message)
    };
};