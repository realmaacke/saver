#include "Project/Object.hpp"
#include "Output/Output.hpp"
#include "openssl/evp.h"
#include "openssl/types.h"
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <optional>
#include <sstream>
#include <vector>

namespace fs = std::filesystem;

void Object::create_obj_directory(const fs::path& root_dir) {
    if (root_dir.empty()) {
        Output::error("Root dir is not set.");
        return;
    }

    this->obj_dir = fs::path(root_dir / "/objects/");

    if (!fs::exists(this->obj_dir)) {
        fs::create_directories(this->obj_dir);
    }
};

std::string Object::transform_file(const fs::path& path) {
    if (!fs::exists(path)) {
        Output::error("Cant read file, it does not exist");
        return {};
    }
    
    // read file into str
    std::ifstream file(path);
    std::stringstream buffer;
    buffer << file.rdbuf();

    // return prefix + file.
    return "object" + buffer.str();
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


std::optional<fs::path> Object::object_path(const std::string& hash) {
    fs::path obj_path = fs::path(this->obj_dir) / hash.substr(0, 2) / hash.substr(2);

    if (fs::exists(obj_path)) {
        Output::print("Identical file already exists");
        return std::nullopt;
    }

    return obj_path;
}

void Object::store_object(const std::string& hash, const std::string& content) {
    std::optional<fs::path> obj_path = this->object_path(hash);

    if (!obj_path.has_value()) {
        return;
    }

    // Create directories if not exists.
    fs::create_directories(obj_path->parent_path());

    if (fs::exists(obj_path->generic_string())) {
        return;
    }


    // TODO: Implement the zlib compression
    std::ofstream outStream(obj_path.value(), std::ios::binary);
    outStream << content;
}