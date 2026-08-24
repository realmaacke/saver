#include "Shipper/Sender.hpp"
#include <string>
#include <nlohmann/json.hpp>


void Sender::setToken(const std::string& token) {
    this->token = token;
}

void Sender::setBaseUrl(const std::string& url) {
    this->baseUrl = url;
}

std::string Sender::request(
    const std::string& method,
    const std::string& path,
    const std::string& body,
    bool useAuth
) {
        CURL* curl = curl_easy_init();
        if (!curl) {
            throw std::runtime_error("failed to init curl");
        }

        std::string url = baseUrl + path;
        std::string response;
        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        if (useAuth && !token.empty()) {
            std::string authHeader = "Authorization: Bearer " + token;
            headers = curl_slist_append(headers, authHeader.c_str());
        }

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        if (method == "POST") {
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
        }

        CURLcode res = curl_easy_perform(curl);
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);

        if (res != CURLE_OK) {
            throw std::runtime_error(curl_easy_strerror(res));
        }

        return response;

}


size_t Sender::writeCallback(char* ptr, size_t size, size_t nmemb, void* userdata) {
    std::string* out = static_cast<std::string*>(userdata);
    out->append(ptr, size * nmemb);
    return size * nmemb;
}