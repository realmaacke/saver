#pragma once

#include "Project/Project.hpp"
#include "Sender/Sender.hpp"
#include "User.hpp"
#include <memory>

class Service {
public:
    static Service& instance() {
        static Service ctx;
        return ctx;
    }

    Project& proj() { return *m_project; }
    Sender& send() { return *m_sender; }
    User& usr() { return *m_user; }

    Service(const Service&) = delete;
    Service& operator=(const Service&) = delete;

private:
    Service() {
        m_sender = std::make_unique<Sender>();
        m_project = std::make_unique<Project>();
        m_user = std::make_unique<User>();    
    }

    std::unique_ptr<Project> m_project;
    std::unique_ptr<Sender> m_sender;
    std::unique_ptr<User> m_user;
};