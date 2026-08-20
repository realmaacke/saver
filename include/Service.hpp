#pragma once
#include <memory>

#include "Project/Project.hpp"
#include "storage.hpp"

// Singleton class that holds all connections.
class Service {
public:
    static Service& instance() {
        static Service ctx;
        return ctx;
    }

    Project& proj() { return *m_project; }
    Storage& store() { return *m_storage; }
    Service(const Service&) = delete;
    Service& operator=(const Service&) = delete;

private:
    Service() {
        m_project = std::make_unique<Project>();
        m_storage = std::make_unique<Storage>();
    }
    std::unique_ptr<Project> m_project;
    std::unique_ptr<Storage> m_storage;
};