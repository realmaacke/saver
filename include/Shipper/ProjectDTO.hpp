#pragma once
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

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

struct ObjectDTO {
    std::string hash;
    std::optional<std::string> mode;
    std::string content;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(
        ObjectDTO,
        hash,
        mode,
        content
    )
};

struct CommitToProject {
    struct Request {
        std::string commit_hash;
        std::string chapter;
        std::vector<ObjectDTO> blobs;

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(
            Request,
            commit_hash,
            chapter,
            blobs
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