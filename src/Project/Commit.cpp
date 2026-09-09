#include "Project/Commit.hpp"
#include "Project/Object.hpp"
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