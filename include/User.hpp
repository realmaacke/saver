#pragma once
#include <string>
class User {

public:
    int connectUser(const std::string& username, const std::string& password);
    bool alreadyConnectedUser();
    
private:
    bool m_alreadyLoggedIn = false;
};