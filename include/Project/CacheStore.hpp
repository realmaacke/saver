#pragma once
#include <filesystem>
#include <string>

namespace fs = std::filesystem;
class CacheStore {
public:
    CacheStore();
    std::string store(const fs::path& file);
    std::string store_bytes(const std::string& content);

    
    std::string sha256_file(const fs::path& file);
    std::string sha256_bytes(const std::string& content);
    
    fs::path path_for(const std::string& hash) const;
    bool contains(const std::string& hash) const;

    std::string digest_to_hex(const unsigned char* digest, unsigned int len);

private:
    fs::path objects_dir_ = ".saver/objects";

};