#pragma once
#include <string>
class User {

public:
    // Login/Logout
    int connectUser(const std::string& username, const std::string& password);
    int disconnectUser();
    // Status of user.
    bool alreadyConnectedUser();
    
private:
    bool m_alreadyLoggedIn = false;
};