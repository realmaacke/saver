#include "Project/CacheStore.hpp"
#include "openssl/evp.h"
#include "openssl/types.h"
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <vector>
#include <sstream>
#include <iomanip>
#include <string>

namespace fs = std::filesystem;

/**
* Did not write this function
* It hashes files anyway.
* Very unreadable.
*/
std::string CacheStore::sha256_file(const fs::path& file) {
    std::ifstream in(file, std::ios::binary);
    if (!in) {
        throw std::runtime_error("sha256_file: cannot open " + file.string());
    }
 
    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    if (!ctx || EVP_DigestInit_ex(ctx, EVP_sha256(), nullptr) != 1) {
        EVP_MD_CTX_free(ctx);
        throw std::runtime_error("sha256_file: failed to init digest");
    }
 
    std::vector<char> buf(1 << 16);
    while (in.read(buf.data(), static_cast<std::streamsize>(buf.size())) || in.gcount() > 0) {
        EVP_DigestUpdate(ctx, buf.data(), static_cast<size_t>(in.gcount()));
    }
 
    unsigned char digest[EVP_MAX_MD_SIZE];
    unsigned int digest_len = 0;
    EVP_DigestFinal_ex(ctx, digest, &digest_len);
    EVP_MD_CTX_free(ctx);
 
    return digest_to_hex(digest, digest_len);
}

std::string CacheStore::digest_to_hex(const unsigned char* digest, unsigned int len) {
    std::ostringstream oss;
    for (unsigned int i = 0; i < len; ++i) {
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(digest[i]);
    }
    return oss.str();
}


std::string CacheStore::store_bytes(const std::string& content) {
    std::string hash = sha256_bytes(content);
    fs::path dest = path_for(hash);
 
    if (!fs::exists(dest)) {
        fs::create_directories(dest.parent_path());
        std::ofstream out(dest, std::ios::binary);
        out << content;
    }
 
    return hash;
}

std::string CacheStore::sha256_bytes(const std::string& content) {
    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    if (!ctx || EVP_DigestInit_ex(ctx, EVP_sha256(), nullptr) != 1) {
        EVP_MD_CTX_free(ctx);
        throw std::runtime_error("sha256_bytes: failed to init digest");
    }
 
    EVP_DigestUpdate(ctx, content.data(), content.size());
 
    unsigned char digest[EVP_MAX_MD_SIZE];
    unsigned int digest_len = 0;
    EVP_DigestFinal_ex(ctx, digest, &digest_len);
    EVP_MD_CTX_free(ctx);
 
    return digest_to_hex(digest, digest_len);
}



CacheStore::CacheStore(){
    fs::create_directories(this->objects_dir_);
}

fs::path CacheStore::path_for(const std::string& hash) const {
    return objects_dir_ / hash.substr(0, 2) / hash.substr(2);
}

bool CacheStore::contains(const std::string& hash) const {
    return fs::exists(path_for(hash));
}

std::string CacheStore::store(const fs::path& file) {
    std::string hash = sha256_file(file);
    fs::path dest = path_for(hash);
 
    if (!fs::exists(dest)) {
        fs::create_directories(dest.parent_path());
        fs::copy_file(file, dest, fs::copy_options::overwrite_existing);
    }
 
    return hash;
}
