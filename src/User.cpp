#include "include/User.hpp"
#include "Environment.hpp"
#include "INIStorage.hpp"
#include "Output/Codes/ErrorCodes.hpp"
#include "Output/Codes/InformationCodes.hpp"
#include "Output/Output.hpp"
#include "Prompt.hpp"
#include "Sender/DTO/UserDTO.hpp"
#include "Service.hpp"
#include "nlohmann/json.hpp"
#include <optional>
#include <string>

void User::setConnected(bool value) {
    this->is_connected = value;
}

bool User::isConnected() {
    std::string cfgPath = LocalConfigFile();

    INIStorage appStorage(cfgPath);
    appStorage.loadStorage();
    std::optional<std::string> token = appStorage.getValue("API_TOKEN").value_or("undefined");

    if (!token.has_value()) { return false; }
    if (token.has_value() && token.value() == "undefined") { return false; }

    Service::instance().send().setToken(token.value());
    
    UserVerify::Response res = Service::instance().send().get<UserVerify::Response>(
        "auth/renew",
        true
    );

    return res.success;
}

int User::connectUser() {
    if(this->is_connected) {
        Output::info(InfoCode::ALREADY_LOGGED_IN);
        return 0;
    }

    std::string username = prompt("Username");
    std::string password = hidden_prompt("Password");

    ConnectUserDTO::Request const body{username, password};
    ConnectUserDTO::Response const res = Service::instance().send().post<ConnectUserDTO::Response>
    (
        "auth/connect",
        body,
        false
    );

    if (!res.success) {
        Output::info(InfoCode::COULD_NOT_LOG_IN);
        return 0;
    }

    if (!res.token.is_string()) {
        Output::error(ErrorCode::AUTH_FAILED_INTERNAL, __FUNCTION__);
        return 1;
    }

    INIStorage storage(LocalConfigFile());
    storage.loadStorage();
    storage.updateStorage("API_TOKEN", res.token.get<std::string>());
    storage.saveStorage();

    Output::info(InfoCode::SUCCESSFULY_LOGGED_IN);
    return 0;
}

int User::disconnectUser() {
    INIStorage storage(LocalConfigFile());
    storage.loadStorage();
    storage.updateStorage("API_TOKEN", "undefined");
    storage.saveStorage();

    Output::info(InfoCode::SUCCESFULY_LOGGED_OUT);
    return 0;
}

UserInfo User::getUser() {
    // querry for user here.
    UserInfo user;

    RetriveUser::Response res = Service::instance().send().get<RetriveUser::Response>
    (
        "auth/getUserInfo",
        true
    );

    if (!res.success) {
        Output::error(ErrorCode::AUTH_FAILED_INTERNAL, __FUNCTION__);
        return user;
    }
    
    try {
        user.user_id = res.data.at("user_id").get<int>();
        user.username = res.data.at("username").get<std::string>();
    } catch (const nlohmann::json::exception&) {
        Output::error(ErrorCode::AUTH_FAILED_INTERNAL, __FUNCTION__);
        return UserInfo{};
    }

    return user;
}