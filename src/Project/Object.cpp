#include "Project/Object.hpp"
#include "Output/Output.hpp"
#include "Project/Tree.hpp"
#include "Shipper/ProjectDTO.hpp"
#include "openssl/evp.h"
#include "openssl/types.h"
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <ios>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

void Object::create_obj_directory(const fs::path& root_dir) {
    this->obj_dir = fs::path(root_dir / ".saver/objects/");

    if (!fs::exists(this->obj_dir.parent_path())) {
        fs::create_directories(this->obj_dir);
    }
};

void Object::create_index_file(const fs::path& root_dir) {
    std::ofstream out(root_dir / ".saver/index");
    out << "" << std::endl;
}

fs::path Object::retrive_obj_dir() {
    if (this->obj_dir.empty()) {
        Output::error("Object path is empty");
        return "";
    }
    return this->obj_dir;
}

std::string Object::transform_file(const fs::path& path) {
    if (!fs::exists(path)) {
        Output::error("Cant read file, it does not exist");
        return {};
    }
    
    // read file into str
    std::ifstream file(path, std::ios::binary);
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();

    std::string prefixes = "blob " + std::to_string(content.size()) + '\0';
    return prefixes + content;
}

std::string Object::sha256(const std::string& data) {
    // hash
    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hash_len = 0;

    // the hashing itself.
    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(ctx, EVP_sha256(), nullptr);
    EVP_DigestUpdate(ctx, data.data(), data.size());
    EVP_DigestFinal_ex(ctx, hash, &hash_len);
    EVP_MD_CTX_free(ctx);

    std::stringstream stream;

    for (size_t i = 0; i < hash_len; i++) {
        stream << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }

    return stream.str();
}

std::string Object::base64_encode(const std::string& data) {
    if (data.empty()) {
        return "";
    }

    // Worst-case output size for base64: 4 * ceil(n/3) + null terminator
    size_t max_len = 4 * ((data.size() + 2) / 3) + 1;
    std::vector<unsigned char> out(max_len);

    int written = EVP_EncodeBlock(
        out.data(),
        reinterpret_cast<const unsigned char*>(data.data()),
        static_cast<int>(data.size())
    );

    return std::string(reinterpret_cast<char*>(out.data()), written);
}

std::string Object::get_permissions(const fs::path& path) {
    fs::perms p = fs::status(path).permissions();

    bool can_execute = (p & fs::perms::owner_exec) != fs::perms::none;

    if (can_execute) {
        return "100755";
    } 
    return "100644";
}


void Object::update_index(
    const fs::path& path,
    const std::string& permissions,
    const std::string& hash
) {
    std::vector<IndexNode> staged = this->get_index();

    for (const IndexNode& node : staged) {
        if (node.path == path && node.permissions == permissions && node.hash == hash) {
            return;
        }
    }

    std::ofstream index(this->index_path, std::ios_base::app |std::ios_base::out);
    index << permissions + " " + path.generic_string() + " " + hash + "\n";
}

std::vector<IndexNode> Object::get_index() {
    std::vector<IndexNode> nodes;
    Output::debug("path", this->index_path);
    std::ifstream index(this->index_path);
    std::string line;

    if (!index.is_open()) {
        Output::error("Failed to open index file");
        return nodes;
    }

    while(std::getline(index, line)) {
        std::istringstream iss(line);
        IndexNode node;
        

        iss >> node.permissions >> node.path >> node.hash;
        nodes.push_back(node);
    }
    return nodes;
}

void Object::reset_index() {
    std::ofstream index(this->index_path, std::ofstream::trunc);
    index << "";
    index.close();
}

std::string Object::get_object_type(const fs::path& path) {
    if (!fs::exists(path)) {
        Output::error("get_object_type(), unknown path");
        return "";
    }

    std::ifstream obj_file(path, std::ios::in);
    std::string first_line;
    std::getline(obj_file, first_line);

    size_t space_separator;
    space_separator = first_line.find_first_of(' ');

    if (space_separator != std::string::npos) {
        return first_line.substr(0, space_separator);
    } else {
        Output::error("get_object_type(), could not find space separator");
        return "";
    }
}

std::optional<fs::path> Object::object_path(const std::string& hash) {
    fs::path obj_path = fs::path(this->obj_dir) / hash.substr(0, 2) / hash.substr(2);
    if (fs::exists(obj_path)) {
        Output::print("Identical file already exists");
        return std::nullopt;
    }
    return obj_path;
}

void Object::set_paths(
    const fs::path& root_dir
) {
    this->obj_dir = fs::path(root_dir / ".saver/objects");
    this->index_path = fs::path(root_dir / ".saver/index");
}

void Object::store_object(const std::string& hash, const std::string& content) {
    std::optional<fs::path> obj_path = this->object_path(hash);

    if (!obj_path.has_value()) {
        return;
    }
    fs::create_directories(obj_path->parent_path());

    std::ofstream outStream(*obj_path, std::ios::trunc);

    if (!outStream.is_open()) {
        Output::print("Failed to create object");
        return;
    }

    outStream << content;

    // // TODO: Implement the zlib compression
}

std::string Object::retrive_blob(const std::string& hash) {
    fs::path obj_path = fs::path(this->obj_dir / hash.substr(0, 2) / hash.substr(2));
    std::string line;

    std::ifstream obj_file(obj_path, std::ios::in | std::ios::binary);

    if (!obj_file.is_open()) {
        Output::error("blob_to_bytes(), could not open file");
        return "";
    }
    std::ostringstream buffer;
    buffer << obj_file.rdbuf();
    return buffer.str();
}

ObjectDTO Object::obj_to_dto(TreeNode& node) {
    ObjectDTO dto;
    dto.hash = node.hash;
    dto.mode = node.permissions;
    if (node.blob) {
        dto.content = this->base64_encode(*node.blob);
    }
    return dto;
}

std::vector<ObjectDTO> Object::array_obj_to_dto(std::vector<TreeNode>& nodes) {
    std::vector<ObjectDTO> ObjectDTO_array;

    for(TreeNode& node : nodes) {
        ObjectDTO_array.emplace_back(this->obj_to_dto(node));
    }
    return ObjectDTO_array;
}