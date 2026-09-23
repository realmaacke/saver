#pragma once
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <string>
#include <variant>
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


struct upload {
    struct Start {
        std::string type = "start";
        std::string commit_hash;
        std::string chapter;
        NLOHMANN_DEFINE_TYPE_INTRUSIVE(
            Start,
            type,
            commit_hash,
            chapter
        );
    };

    struct Chunk {
        std::string type = "chunk";
        std::string hash;
        std::optional<std::string> mode;
        std::string content;

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(
            Chunk,
            type,
            hash,
            mode,
            content
        )
    };

    struct Done {
        std::string type = "done";
        bool success;
        std::string message;
        NLOHMANN_DEFINE_TYPE_INTRUSIVE(
            Done,
            type,
            success,
            message
        );
    };

    using Line = std::variant<Start, Chunk, Done>;

    struct Request {
        Line line;

        static Line parseLine(const std::string& raw) {
            nlohmann::json json = nlohmann::json::parse(raw);
            std::string type = json.at("type").get<std::string>();

            if (type == "start") return json.get<Start>();
            if (type == "chunk") return json.get<Chunk>();
            if (type == "done")  return json.get<Done>();

            throw std::runtime_error("unknown type: " + type);
        }
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
        );
    };
};