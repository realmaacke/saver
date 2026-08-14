#pragma once
 
#include <curl/curl.h>
#include <string>
 
class Curl {
    private:
 
    CURL* init();
    void perform(CURL* handle, curl_slist* headers = nullptr);
    static size_t write_cb(char* ptr, size_t size, size_t nmemb, void* userdata);
protected:
    std::string get(const std::string& url);
    std::string post(const std::string& url, const std::string& body, const std::string& content_type = "");
    std::string post_with_json(const std::string& url, const std::string& json_body);
    std::string post_with_form(const std::string& url, const std::string& form_body);
};
