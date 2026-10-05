#include "Project/Content/Object.hpp"
#include "Output/Codes/ErrorCodes.hpp"
#include "Output/Output.hpp"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>


Object::Object(const fs::path& object_dir) {
    this->object_dir = object_dir;
}

std::string Object::file_to_blob(
    const fs::path& path,
    const std::string& prefix,
    const std::ios::openmode mode
) {
    if (!fs::exists(path)) {
        Output::error(ErrorCode::INVALID_PATH, __FUNCTION__);
        return {};
    }

    std::ifstream file(path, mode);
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();

    std::string prefixes = prefix + " " + std::to_string(content.size()) + '\0';
    return prefixes + content;
}

void Object::blob_to_file(
    const std::string& hash,
    const std::string& content
) {
    fs::path subdir = fs::path(this->object_dir) / hash.substr(0, 2);
    fs::path hash_path = subdir / hash.substr(2);
    
    if (fs::exists(hash_path)) {
        return;
    }
    
    fs::create_directories(subdir);

    std::ofstream outStream(hash_path, std::ios::binary);

    if (!outStream.is_open()) {
        Output::error(ErrorCode::FILE_ERROR, __FUNCTION__);
        return;
    }

    outStream << content;
}


std::string Object::file_permissions(const fs::path& path) {
    fs::perms p = fs::status(path).permissions();

    if ((p & fs::perms::owner_exec) != fs::perms::none) {
        return "100755";
    }
    return "100644";
}

std::string Object::retrive_stored_blob(const std::string& hash) {
    fs::path obj_path = fs::path(this->object_dir / hash.substr(0, 2) / hash.substr(2));
    std::string line;

    std::ifstream blob_file(obj_path, std::ios::in | std::ios::binary);

    if (!blob_file.is_open()) {
        Output::error(ErrorCode::FILE_ERROR, __FUNCTION__);
        return {};
    }

    std::ostringstream buffer;
    buffer << blob_file.rdbuf();
    return buffer.str();
}