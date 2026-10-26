#include "include/Sender/Sender.hpp"
#include "INIStorage.hpp"
#include "Environment.hpp"
#include "Output/Codes/ErrorCodes.hpp"
#include "Output/Output.hpp"
#include "Service.hpp"
#include "curl/system.h"
#include <optional>
#include <string>
#include <nlohmann/json.hpp>


void Sender::setToken(const std::string& token) {
    this->token = token;
}

/**
 * Called from main.cpp
 * Sets API-URL from config.
 */
void Sender::setBaseUrl() {
    INIStorage storage(LocalConfigFile());

    storage.loadStorage();

    std::string url = storage.getValue("API_URL").value_or("");
    std::string port = storage.getValue("API_PORT").value_or("");
    
    std::erase(url, '"');
    std::erase(port, '"');

    if (url.empty() || port.empty()) {
        Output::error(ErrorCode::INTERNAL_VARS_UNSET, __FUNCTION__);
        return;
    }

    // Removes trailing slash from url.
    if (url.ends_with("/")) {
        url = url.substr(0, url.size() - 1);
    }

    this->baseUrl = url + ":" + port + "/";
}

int Sender::progressCallback(
    void* clientp,
    curl_off_t downloadTotal, curl_off_t downloadNow,
    curl_off_t uploadTotal, curl_off_t uploadNow
) {
    ProgressContext* ctx = static_cast<ProgressContext*>(clientp);
    if (ctx->callback) {
        ctx->callback(uploadNow, uploadTotal);
    }
    return 0;
}

std::string Sender::request(
    const std::string& method,
    const std::string& path,
    const std::string& body,
    bool useAuth,
    std::function<void(curl_off_t, curl_off_t)> progress,
    const std::string& contentType
) {
        this->setBaseUrl();

        CURL* curl = curl_easy_init();
        if (!curl) {
            throw std::runtime_error("failed to init curl");
        }

        std::string url = this->baseUrl + path;

        std::string response;
        struct curl_slist* headers = nullptr;

        std::string contentTypeHeader = "Content-Type: " + contentType;

        headers = curl_slist_append(headers, contentTypeHeader.c_str());
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

        ProgressContext ctx{progress};
        if (progress) {
            curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
            curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, progressCallback);
            curl_easy_setopt(curl, CURLOPT_XFERINFODATA, &ctx);
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