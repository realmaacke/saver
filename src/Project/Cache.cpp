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

    CacheType cacheType;
    cacheType.path = path;

    this->cache_store.emplace_back(cacheType);
}