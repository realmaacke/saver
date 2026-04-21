#include <iostream>

#include "sender.hpp"

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

std::string createManifest(
    const std::vector<fs::path>& files,
    const fs::path& base_dir
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


int Sender::send(const SendRequest& req) {
    CURL* curl = curl_easy_init();

    if (!curl) return 1;

    std::string response;

    curl_mime* mime = curl_mime_init(curl);

    if (!req.files.empty()) {
        for (size_t i = 0; i < req.files.size(); i++) {
            const auto& file = req.files[i];

            if (!fs::exists(file) || !fs::is_regular_file(file)) {
                std::cerr << "Invalid file: " << file << std::endl;
                curl_mime_free(mime);
                curl_easy_cleanup(curl);
                return 1;
            }

            curl_mimepart* part = curl_mime_addpart(mime);
            if (!part) {
                std::cerr << "Failed to create mime part" << std::endl;
                curl_mime_free(mime);
                curl_easy_cleanup(curl);
                return 1;
            }

            std::string field_name = "file" + std::to_string(i);

            curl_mime_name(part, field_name.c_str());
            curl_mime_filedata(part, file.string().c_str());
            curl_mime_filename(part, file.filename().string().c_str());
        }
    }

    // Adding manifest if include is enabled.
    if (req.include_manifest) {
        std::string manifest = req.manifest_json;

        if (manifest.empty() && !req.files.empty()) {
            manifest = createManifest(req.files, req.base_dir);
        }

        if (!manifest.empty()) {
            curl_mimepart* manifest_part = curl_mime_addpart(mime);

            if (!manifest_part) {
                std::cerr << "Failed to create manifest" << std::endl;
                curl_mime_free(mime);
                curl_easy_cleanup(curl);
                return 1;
            }
            curl_mime_name(manifest_part, "manifest");
            curl_mime_data(manifest_part, manifest.c_str(), manifest.size());
            curl_mime_type(manifest_part, "application/json");
        }
    }

    // Appending headers to list.
    struct curl_slist* header_list = nullptr;
    for (const auto& header : req.headers) {
        header_list = curl_slist_append(header_list, header.c_str());
    }

    // Setup of curl with options.
    curl_easy_setopt(curl, CURLOPT_URL, req.url.c_str());
    curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, req.timeout);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    // Adding the header list
    if (header_list) {
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);
    }

    CURLcode res = curl_easy_perform(curl);

    // Appending http code to long.
    long http_code = 0;;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, & http_code);

    // cleanup
    curl_slist_free_all(header_list);
    curl_mime_free(mime);
    curl_easy_cleanup(curl);

    // check for ok = false
    if (res != CURLE_OK) {
        std::cerr << "Could not send to remote: " << curl_easy_strerror(res) << std::endl;
        return 1;
    }

    std::cout << "Sent to remote, code: " << http_code << std::endl << response << std::endl;
    return 0;
}