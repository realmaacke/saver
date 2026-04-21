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
};

class Sender {
public:
    Sender() = default;

    // int sendFile(
    //     const std::string& remote,
    //     const fs::path& base_dir,
    //     const fs::path& file
    // );

    // int sendMultipleFiles(
    //     const std::string& remote,
    //     const fs::path& base_dir,
    //     const std::vector<fs::path>& files
    // );


    int send(const SendRequest& req);

    // bool authenticateLogin(
    //     fs::path& key_path
    // );
private:
};