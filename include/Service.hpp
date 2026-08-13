#pragma once
#include <memory>

#include "Project/Project.hpp"

// Singleton class that holds all connections.
class Service {
public:
    static Service& instance() {
        static Service ctx;
        return ctx;
    }

    Project& proj() { return *m_project; }
    Service(const Service&) = delete;
    Service& operator=(const Service&) = delete;

private:
    Service() {
        m_project = std::make_unique<Project>();
    }
    std::unique_ptr<Project> m_project;
};