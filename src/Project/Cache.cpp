#include "Project/Cache.hpp"
#include "Project/Object.hpp"

Cache::Cache(Object* object) {
    this->object = object;
}

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


    std::string bytes = this->object->transform_file(path);
    std::string hash = this->object->sha256(bytes);

    // creates a file in /objects/hash    
    this->object->store_object(hash, bytes);
    
    // Store the info
    CacheType cacheType;
    cacheType.path = path;
    cacheType.hash = hash;

    this->cache_store.emplace_back(cacheType);
}

