#pragma once
#include "Project/Object.hpp"
#include <optional>

struct CommitObject {
    std::string tree;
    std::string message;
    std::optional<std::string> parent;
    std::string author;
};

class Commit {
public:

    Commit(Object* object);

    std::string create_commit_object(
        const std::string& hash,
        const std::string& parent,
        const std::string& message,
        const std::string& author
    );

    CommitObject unfold_commit_object(const std::string& hash);

private:
    Object* object;
};