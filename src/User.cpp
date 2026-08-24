#include "User.hpp"
#include <string>
#include "Output/Output.hpp"
#include "Service.hpp"
#include "Shipper/UserDTO.hpp"
int User::connectUser(const std::string& username, const std::string& password) {
    Output::print("username: " + username);
    Output::print("password: " + password);

    ConnectUser::Request body {username, password};

    ConnectUser::Response res = Service::instance().send()
        .post<ConnectUser::Response, ConnectUser::Request>(
            "auth/connect",
            body,
            false
    );

    nlohmann::json j = body;
    Output::print("sending: " + j.dump()); // temp debug

    if (res.success && res.token.is_string()) {
        Service::instance().send().setToken(res.token);
        return 0;
    }

    Output::error(res.message);
    return 1;
}

bool User::lookForToken() {

}

void User::setUser(const std::string& userId, const std::string& username, const std::string& token) {

}