#pragma once
#include "Output/ErrorCode.hpp"
#include "Output/Output.hpp"
#include "Shipper/ProjectDTO.hpp"
#include "curl/system.h"
#include <curl/curl.h>
#include <functional>
#include <string>
#include <nlohmann/json.hpp>
#include <vector>

class Sender {
public:
    void setToken(const std::string& token);
    void setBaseUrl();

    template<typename ResponseDTO>
    ResponseDTO get(const std::string& path, bool useAuth) {
        std::string raw = this->request("GET", path, "", useAuth);
        return parse_response<ResponseDTO>(raw);
    }

    template<typename ResponseDTO, typename RequestDTO>
    ResponseDTO post(
        const std::string& path,
        const RequestDTO body,
        bool useAuth,
        std::function<void(curl_off_t, curl_off_t)> progress = nullptr
    ) {
        nlohmann::json j = body;
        std::string raw = this->request("POST", path, j.dump(), useAuth, progress);
        return parse_response<ResponseDTO>(raw);
    }

    template<typename ResponseDTO>
    ResponseDTO postNDJSON(
        const std::string& path,
        const std::vector<upload::Line>& lines,
        bool useAuth,
        std::function<void(curl_off_t, curl_off_t)> progress = nullptr
    ) {
        std::string body;
        for (const auto& line : lines) {
            std::visit([&body](auto&& l) {
                body += nlohmann::json(l).dump() + "\n";
            }, line);
        }
        std::string raw = this->request(
            "POST",
            path,
            body,
            useAuth,
            progress,
            "application/x-ndjson"
        );
        return parse_response<ResponseDTO>(raw);   
    }

private:
    // for loading bar
    struct ProgressContext {
        std::function<void(curl_off_t, curl_off_t)> callback;
    };

    static int progressCallback(
        void* clientp,
        curl_off_t downloadTotal, curl_off_t downloadNow,
        curl_off_t uploadTotal, curl_off_t uploadNow
    );

    template<typename ResponseDTO>
    ResponseDTO parse_response(const std::string& raw) {
        try {
            return nlohmann::json::parse(raw).get<ResponseDTO>();
        } catch (const nlohmann::json::parse_error&) {
            Output::error(ErrorType::NON_JSON_REQUEST, __FUNCTION__);
            // remove the line below once saver is stable enough.
            Output::print(raw.substr(0, 200));
            throw;
        }
    }

    std::string baseUrl;
    std::string token;

    static size_t writeCallback(
        char* ptr,
        size_t size,
        size_t nmemb,
        void* userdata
    );

    std::string request(const std::string& method,
        const std::string& path,
        const std::string& body,
        bool useAuth,
        std::function<void(curl_off_t, curl_off_t)> progress = nullptr,
        const std::string& contentType = "application/json"
    );
};