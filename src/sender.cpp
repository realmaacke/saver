#include "sender.hpp"
#include <filesystem>
#include <iostream>
#include <fstream>
#include <cstdio>

#include <sstream>
#include <stdexcept>

#include <curl/curl.h>
#include <string>

namespace fs = std::filesystem;

static size_t write_callback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t total = size * nmemb;
    static_cast<std::string*>(userp)->append(static_cast<char*>(contents), total);
    return total;
}

static std::string json_escape(const std::string& s) {
    std::string out;
    out.reserve(s.size());

    for (char c : s) {
        switch (c) {
            case '\"': out += "\\\""; break;
            case '\\': out += "\\\\"; break;
            case '\b': out += "\\b"; break;
            case '\f': out += "\\f"; break;
            case '\n': out += "\\n"; break;
            case '\r': out += "\\r"; break;
            case '\t': out += "\\t"; break;
            default:   out += c; break;
        }
    }
    return out;
}

// bool Sender::authenticateLogin(fs::path& key_path) {
//     std::ifstream file(key_path);
//     std::stringstream buffer;

//     buffer << file.rdbuf();

//     std::string key = buffer.str();

//     std::unique_ptr<CURL, decltype(&curl_easy_cleanup)> curl(curl_easy_init(), curl_easy_cleanup);

//     if (curl) {
//         curl_easy_setopt(curl.get(), CURLOPT_URL, "http://localhost:9019");
//         curl_easy_setopt(curl.get(), CURLOPT_POST, 1L);
//         curl_easy_setopt(curl.get(), CURLOPT_POSTFIELDS, key.c_str());
//         curl_easy_setopt(curl.get(), CURLOPT_POSTFIELDSIZE, key.size());

//     }

    
//     std::unique_ptr<CURL, decltype(&curl_easy_cleanup)> curl(curl_easy_init(), curl_easy_cleanup);
    

//     // make request to server
//     // if key == key on server

//     // allow

//     // else error msg + disallow

//     return false;
// }

int Sender::sendFile(
    const std::string& remote,
    const fs::path& base_dir,
    const fs::path& file
) {
    return sendMultipleFiles(remote, base_dir, std::vector<fs::path>{file});
}

std::string createManifest(
    const std::vector<fs::path>& files,
    const fs::path& base_dir,
    curl_mime* mime
) {
    std::ostringstream manifest;
    bool first = true;

    manifest << "[";

    for (size_t i = 0; i < files.size(); i++) {
        const auto& file = files[i];

        if (!fs::exists(file) || !fs::is_regular_file(file)) {
            throw std::runtime_error("Saver could not load file: " + file.string());
        }

        fs::path rel = fs::relative(file, base_dir);
        std::string field_name = "file" + std::to_string(i);

        curl_mimepart* file_part = curl_mime_addpart(mime);

        if (!file_part) {
            throw std::runtime_error("Failed to create mime part for: " + file.string());
        }

        curl_mime_name(file_part, field_name.c_str());
        curl_mime_filedata(file_part, file.string().c_str());
        curl_mime_filename(file_part, file.filename().string().c_str());

        if (!first) {
            manifest << ", ";
        }
        first = false;

        manifest << "{"
                 << "\"field\":\"" << json_escape(field_name) << "\","
                 << "\"path\":\"" << json_escape(rel.generic_string()) << "\""
                 << "}";
    }

    manifest << "]";
    return manifest.str();
}


int Sender::sendMultipleFiles(
    const std::string& remote,
    const fs::path& base_dir,
    const std::vector<fs::path>& files
) {

    if(files.empty()) {
        std::cerr << "Saver could not find any files to upload" << std::endl;
        return 1;
    }

    CURL* curl = curl_easy_init();
    if (!curl) {
        std::cerr << "Failed to initialize sender" << std::endl;
        return 1;
    }

    std::string response;
    curl_mime* mime = curl_mime_init(curl);
    std::string manifest_json;
    try {
        manifest_json = createManifest(files, base_dir, mime);
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        curl_mime_free(mime);
        curl_easy_cleanup(curl);
        return 1;
    }

    curl_mimepart* manifest_part = curl_mime_addpart(mime);

    if(!manifest_part) {
        std::cerr << "Failed to create manifest part" << std::endl;
        curl_mime_free(mime);
        curl_easy_cleanup(curl);
        return 1;
    }

    curl_mime_name(manifest_part, "manifest");
    curl_mime_data(manifest_part, manifest_json.c_str(), CURL_ZERO_TERMINATED);
    curl_mime_type(manifest_part, "application/json");

    curl_easy_setopt(curl, CURLOPT_URL, remote.c_str());
    curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);

    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

    curl_mime_free(mime);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        std::cerr << "Upload failed: " << curl_easy_strerror(res) << '\n';
        return 1;
    }

    std::cout << "HTTP " << http_code << '\n';
    std::cout << response << '\n';
    return 0;
}