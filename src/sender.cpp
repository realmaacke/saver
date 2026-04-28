#include <iostream>
#include <optional>

#include "sender.hpp"

namespace fs = std::filesystem;

static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t total = size * nmemb;
    static_cast<std::string *>(userp)->append(static_cast<char *>(contents), total);
    return total;
}

static std::string json_escape(const std::string &s)
{
    std::string out;
    out.reserve(s.size());

    for (char c : s)
    {
        switch (c)
        {
        case '\"':
            out += "\\\"";
            break;
        case '\\':
            out += "\\\\";
            break;
        case '\b':
            out += "\\b";
            break;
        case '\f':
            out += "\\f";
            break;
        case '\n':
            out += "\\n";
            break;
        case '\r':
            out += "\\r";
            break;
        case '\t':
            out += "\\t";
            break;
        default:
            out += c;
            break;
        }
    }
    return out;
}

std::string createManifest(
    const std::vector<fs::path> &files,
    const fs::path &base_dir)
{
    std::ostringstream manifest;
    bool first = true;

    manifest << "[";

    for (size_t i = 0; i < files.size(); i++)
    {
        const auto &file = files[i];

        if (!fs::exists(file) || !fs::is_regular_file(file))
        {
            throw std::runtime_error("Saver could not load file: " + file.string());
        }

        fs::path rel = fs::relative(file, base_dir);
        std::string field_name = "file" + std::to_string(i);

        if (!first)
        {
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

std::optional<SendResponse> Sender::send(const SendRequest &req)
{
    CURL *curl = curl_easy_init();

    if (!curl)
    {
        return std::nullopt;
    }

    std::string response;

    struct curl_slist *header_list = nullptr;
    for (const auto &h : req.headers)
    {
        header_list = curl_slist_append(header_list, h.c_str());
    }

    curl_easy_setopt(curl, CURLOPT_URL, req.url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, req.timeout);

    CURLcode res;
    long http_code = 0;

    if (req.body_type == SendRequest::BodyType::JSON && !req.json_body.empty())
    {
        // JSON

        struct curl_slist *json_headers = nullptr;
        json_headers = curl_slist_append(json_headers, "Content-Type: application/json");

        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, json_headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, req.json_body.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, req.json_body.size());

        res = curl_easy_perform(curl);

        curl_slist_free_all(json_headers);
    }
    else if (req.body_type == SendRequest::BodyType::MULTIPART)
    {

        curl_mime *mime = curl_mime_init(curl);

        // files
        for (size_t i = 0; i < req.files.size(); i++)
        {
            const auto &file = req.files[i];

            if (!fs::exists(file) || !fs::is_regular_file(file))
            {
                std::cerr << "Invalid file: " << file << std::endl;
                curl_mime_free(mime);
                curl_easy_cleanup(curl);

                return std::nullopt;
            }

            curl_mimepart *part = curl_mime_addpart(mime);

            std::string field_name = "file" + std::to_string(i);

            curl_mime_name(part, field_name.c_str());
            curl_mime_filedata(part, file.string().c_str());
            curl_mime_filename(part, file.filename().string().c_str());
        }

        // manifest
        if (req.include_manifest && !req.manifest_json.empty())
        {
            curl_mimepart *m = curl_mime_addpart(mime);
            curl_mime_name(m, "manifest");
            curl_mime_data(m, req.manifest_json.c_str(), req.manifest_json.size());
            curl_mime_type(m, "application/json");
        }

        curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);

        res = curl_easy_perform(curl);

        curl_mime_free(mime);
    }
    else
    {
        // GET
        res = curl_easy_perform(curl);
    }

    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

    curl_slist_free_all(header_list);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK)
    {
        std::cerr << "Request failed: " << curl_easy_strerror(res) << std::endl;
        return std::nullopt;
    }

    SendResponse r;

    r.http_code = http_code;
    r.body = response;
    r.success = true;

    return r;
}