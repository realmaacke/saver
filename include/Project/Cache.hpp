#pragma once
#include <optional>
#include <string>
#include <vector>

class Object;

struct CacheType {
    std::string path;
    std::string hash;
};

class Cache{
public:
    Cache(Object* object);
    std::optional<CacheType> get_from_cache(const std::string& path);
    void add_to_cache(const std::string& path);

private:
    std::vector<CacheType> cache_store;
    Object* object;
};