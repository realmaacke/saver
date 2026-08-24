#include "User.hpp"
#include <string>
#include "Output/Output.hpp"
#include "Service.hpp"
#include "Shipper/UserDTO.hpp"

bool User::alreadyConnectedUser() {
    // Gets saved token
    std::string currentToken = Service::instance().store().getFromConfig("API_TOKEN");

    // Checks if token is legitimate
    if (!currentToken.empty()) {
        Service::instance().send().setToken(currentToken);
        RenewToken::Response token_res = Service::instance().send()
            .get<RenewToken::Response>(
                "auth/renew",
                true
            );
        
        // if it has, use that as user.
        if (token_res.success && token_res.token.is_string()) {
            Service::instance().send().setToken(token_res.token);
            Service::instance().store().addToConfig("API_TOKEN", token_res.token);
            this->m_alreadyLoggedIn = true;
            return true;
        }
    }
    return false;
}

int User::connectUser(const std::string& username, const std::string& password) {
    if (this->alreadyConnectedUser()) {
        Output::print("User already logged in");
        Output::print("If you want to login as another user, use: saver disconnect");
        return 0;
    }


    ConnectUser::Request body {username, password};

    ConnectUser::Response res = Service::instance().send()
        .post<ConnectUser::Response, ConnectUser::Request>(
            "auth/connect",
            body,
            false
    );

    if (res.success && res.token.is_string()) {
        Service::instance().send().setToken(res.token);
        Service::instance().store().addToConfig("API_TOKEN", res.token);
        Output::print("Connection successful!");
        return 0;
    }

    Output::error(res.message);
    return 1;
}