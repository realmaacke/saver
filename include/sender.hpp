#pragma once
#include <filesystem>
#include <vector>
#include <curl/curl.h>
#include <optional>

namespace fs = std::filesystem;

struct SendRequest
{
    std::string url;

    std::vector<fs::path> files;
    fs::path base_dir;

    std::string manifest_json;
    bool include_manifest = true;

    std::vector<std::string> headers;

    long timeout = 30;

    std::string json_body;

    enum class BodyType
    {
        NONE,
        JSON,
        MULTIPART
    };

    BodyType body_type = BodyType::NONE;
};

struct SendResponse
{
    long http_code = 0;
    std::string body;
    bool success = false;
    std::string error;
};

class Sender
{
public:
    Sender() = default;

    std::optional<SendResponse> send(const SendRequest &req);
};