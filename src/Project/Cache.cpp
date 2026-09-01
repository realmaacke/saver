#include "Project/Cache.hpp"

std::optional<CacheType> Cache::get_from_cache(const std::string& path) {
    for (const CacheType& entry : this->cache_store) {
        if (path == entry.path) {
            return entry;
        }
    }
    return std::nullopt;
}

void Cache::add_to_cache(const std::string& path) {
    if (this->get_from_cache(path).has_value()) {
        return;
    }

    std::string bytes = this->object_.transform_file(path);
    std::string hash = this->object_.sha256(bytes);
    // creates a file in /objects/hash    
    this->object_.store_object(hash, bytes);
    
    // Store the info
    CacheType cacheType;
    cacheType.path = path;
    cacheType.hash = hash;

    this->cache_store.emplace_back(cacheType);
}

