#include "Project/Commit.hpp"
#include "Output/Output.hpp"
#include "Project/Object.hpp"
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <string>

Commit::Commit(Object* object) {
    this->object = object;
}

std::string Commit::create_commit_object(
    const std::string& hash,
    const std::string& parent,
    const std::string& message,
    const std::string& author
) {
    std::string content;
    content += "tree " + hash + '\n';

    if (!parent.empty()) {
        content += "parent " + parent + '\n';
    }

    content += "message " + message + '\n';
    content += "author " + author + '\n';

    std::string tagged = "commit " 
        + std::to_string(content.size())
        + '\0'
        + content
    ;

    std::string tagged_hash = this->object->sha256(tagged);
    
    this->object->store_object(tagged_hash, tagged);

    return tagged_hash;
}

CommitObject Commit::unfold_commit_object(const std::string& hash) {
    fs::path object_path = this->object->retrive_obj_dir() / hash.substr(0, 2) / hash.substr(2);
    CommitObject commit_object;

    if (!fs::exists(object_path)) {
        Output::error("unfold_commit_object(), path does not exist");
        return commit_object;
    }

    std::ifstream commit_file(object_path, std::ios::in);

    if (!commit_file.is_open()) {
        Output::error("unfold_commit_object(), could not open file");
        return commit_object;
    }

    std::string line;
    while (std::getline(commit_file, line)) {
        size_t null_pos = line.find('\0');

        if (null_pos != std::string::npos) {
            size_t tree_pos = line.find("tree ", null_pos);
            if (tree_pos != std::string::npos) {
                commit_object.tree = line.substr(tree_pos + 5);
            }
            continue;
        }

        if (line.rfind("parent ", 0) == 0) {
            commit_object.parent = line.substr(7);
        } else if (line.rfind("message ", 0) == 0) {
            commit_object.message = line.substr(8);
        } else if (line.rfind("author ", 0) == 0) {
            commit_object.author = line.substr(7);
        }
    }
    return commit_object;
};