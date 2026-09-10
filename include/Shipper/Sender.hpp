#pragma once
#include "Output/Output.hpp"
#include <curl/curl.h>
#include <string>
#include <nlohmann/json.hpp>

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
    ResponseDTO post(const std::string& path, const RequestDTO body, bool useAuth) {
        nlohmann::json j = body;
        std::string raw = this->request("POST", path, j.dump(), useAuth);
        return parse_response<ResponseDTO>(raw);
    }

private:
    template<typename ResponseDTO>
    ResponseDTO parse_response(const std::string& raw) {
        try {
            return nlohmann::json::parse(raw).get<ResponseDTO>();
        } catch (const nlohmann::json::parse_error&) {
            Output::error("Server returned a non-JSON response: " + raw.substr(0, 200));
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
        bool useAuth
    );
};