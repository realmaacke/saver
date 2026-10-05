#pragma once
#include <optional>
#include <string>
#include <variant>
#include <nlohmann/json.hpp>


struct Upload {
    struct Start {
        std::string type = "start";
        std::string describe;
        std::string chapter;
        NLOHMANN_DEFINE_TYPE_INTRUSIVE(Start, type, describe, chapter);
    };

    struct Chunk {
        std::string type = "chunk";
        std::string hash;
        std::optional<std::string> mode;
        std::string content;

        friend void to_json(nlohmann::json& j, const Chunk& c) {
            j = nlohmann::json{
                {"type", c.type},
                {"hash", c.hash},
                {"content", c.content}
            };
            
            if (c.mode.has_value()) {
                j["mode"] = *c.mode;
            } else {
                j["mode"] = nullptr;
            }
        }

        friend void from_json(const nlohmann::json& j, Chunk& c) {
            j.at("type").get_to(c.type);
            j.at("hash").get_to(c.hash);
            j.at("content").get_to(c.content);

            if (j.contains("mode") && !j["mode"].is_null()) {
                c.mode = j.at("mode").get<std::string>();
            } else {
                c.mode = std::nullopt;
            }
        }

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(Chunk, type, hash, mode, content)
    };

    struct Done {
        std::string type = "done";
        NLOHMANN_DEFINE_TYPE_INTRUSIVE(Done, type);
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
        NLOHMANN_DEFINE_TYPE_INTRUSIVE(Response, success, data, message)
    };
};