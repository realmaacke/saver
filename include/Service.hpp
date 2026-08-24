#pragma once
#include <memory>

#include "Project/Project.hpp"
#include "Shipper/Sender.hpp"
#include "Storage.hpp"
#include "User.hpp"

// Singleton class that holds all connections.
class Service {
public:
    static Service& instance() {
        static Service ctx;
        return ctx;
    }

    User& user() { return *m_user; }
    Project& proj() { return *m_project; }
    Storage& store() { return *m_storage; }
    Sender& send() { return *m_sender; }
    Service(const Service&) = delete;
    Service& operator=(const Service&) = delete;

private:
    Service() {
        m_project = std::make_unique<Project>();
        m_storage = std::make_unique<Storage>();
        m_user = std::make_unique<User>();
        m_sender = std::make_unique<Sender>();
    }
    std::unique_ptr<Project> m_project;
    std::unique_ptr<Storage> m_storage;
    std::unique_ptr<User> m_user;
    std::unique_ptr<Sender> m_sender;
};