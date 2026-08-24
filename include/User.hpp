#pragma once
#include <string>
class User {

public:
    int connectUser(const std::string& username, const std::string& password);
    bool lookForToken();
    
private:
    void setUser(const std::string& userId, const std::string& username, const std::string& token);
    
    std::string userId;
    std::string username;
    std::string token;
};