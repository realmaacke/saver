#pragma once

#include <filesystem>
#include <vector>
struct CacheEntry {
    std::filesystem::path original_path;
    std::filesystem::path cached_path;
};

class Cache {
public:
    Cache();
    void add(const std::filesystem::path& file);
    const std::vector<CacheEntry>& entries() const;
    void clear();

private:
    void add_file(
        const std::filesystem::path& file,
        const std::filesystem::path& relative_path
    );

    std::filesystem::path cache_dir = ".saver/cache";
    std::vector<CacheEntry> entries_;
};