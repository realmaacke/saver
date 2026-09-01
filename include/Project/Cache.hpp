#pragma once
#include "Project/Object.hpp"
#include <optional>
#include <string>
#include <vector>

struct CacheType {
    std::string path;
    std::string hash;
};

class Cache{
public:
    std::optional<CacheType> get_from_cache(const std::string& path);
    void add_to_cache(const std::string& path);

private:
    std::vector<CacheType> cache_store;
    Object object_;
};