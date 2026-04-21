#pragma once
#include <filesystem>
#include <vector>
#include <curl/curl.h>

namespace fs = std::filesystem;

struct SendRequest {
    std::string url;

    std::vector<fs::path> files;
    fs::path base_dir;

    std::string manifest_json;
    bool include_manifest = true;

    std::vector<std::string> headers;

    long timeout = 30;

    std::string json_body;
};

class Sender {
public:
    Sender() = default;

    int send(const SendRequest& req);
};