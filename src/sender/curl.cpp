#include "sender/curl.hpp"
#include <curl/easy.h>
#include <iterator>


// Inits handler and sets callback.
CURL* Curl::init() {
    CURL* handle = curl_easy_init();
    if (!handle) throw std::runtime_error("curl_easy_init failed");
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, &Curl::write_cb);
    return handle;
}

// Clean up.
void Curl::perform(CURL* handle, curl_slist* headers) {
    CURLcode rc = curl_easy_perform(handle);
    if (headers) curl_slist_free_all(headers);
    curl_easy_cleanup(handle);
    if (rc != CURLE_OK) throw std::runtime_error(curl_easy_strerror(rc));
}

// Callback.
size_t Curl::write_cb(char* ptr, size_t size, size_t nmemb, void* userdata) {
    std::string* out = static_cast<std::string*>(userdata);
    size_t total = size * nmemb;
    out->append(ptr, total);
    return total;
}

// Basic Get.
std::string Curl::get(const std::string& url) {
    CURL* handle = init();
    std::string response;
    curl_easy_setopt(handle, CURLOPT_URL, url.c_str());
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, &response);
    perform(handle);
    return response;
}

//Basic Post
std::string Curl::post(const std::string& url, const std::string& body, const std::string& content_type) {
    CURL* handle = init();
    std::string response;
    if (!content_type.empty()) {
        curl_slist* header = curl_slist_append(nullptr, content_type.c_str());
        curl_easy_setopt(handle, CURLOPT_HTTPHEADER, header);
    }
    curl_easy_setopt(handle, CURLOPT_URL, url.c_str());
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(handle, CURLOPT_POSTFIELDS, body.c_str());
    curl_easy_setopt(handle, CURLOPT_POSTFIELDSIZE, static_cast<long>(body.size()));
    perform(handle);
    return response;
}

std::string Curl::post_with_json(const std::string& url, const std::string& json_body) {
    return this->post(
        url,
        json_body,
        "Content-Type: application/json"
    );
}

std::string Curl::post_with_form(const std::string& url, const std::string& form_body) {
    return this->post(
        url,
        form_body,
        "Content-Type: application/x-www-form-urlencoded"
    );
}