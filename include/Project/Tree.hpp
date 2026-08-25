#pragma once
 
#include <filesystem>
#include <string>
#include <vector>

namespace fs = std::filesystem;
 
struct TreeEntry {
    fs::path path;
    std::string hash;
};
 
class Tree {
public:
    void add(const fs::path& path, const std::string& hash);
 
    // One line per entry: "<hash> <path>\n", sorted by path for determinism.
    std::string serialize() const;
 
    const std::vector<TreeEntry>& entries() const;
 
private:
    std::vector<TreeEntry> entries_;
};
 
