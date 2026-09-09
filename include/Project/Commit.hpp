#pragma once
#include "Project/Object.hpp"

class Commit {
public:

    Commit(Object* object);

    std::string create_commit_object(
        const std::string& hash,
        const std::string& parent,
        const std::string& message,
        const std::string& author
    );
private:
    Object* object;
};