#include "Project/Tree.hpp"

#include <algorithm>
#include <sstream>

namespace fs = std::filesystem;

void Tree::add(const fs::path& path, const std::string& hash) {
    entries_.push_back(TreeEntry{path, hash});
}

std::string Tree::serialize() const {
    std::vector<TreeEntry> sorted = entries_;
    std::sort(sorted.begin(), sorted.end(), [](const TreeEntry& a, const TreeEntry& b) {
        return a.path < b.path;
    });

    std::ostringstream oss;
    for (const TreeEntry& entry : sorted) {
        oss << entry.hash << ' ' << entry.path.generic_string() << '\n';
    }
    return oss.str();
}

const std::vector<TreeEntry>& Tree::entries() const {
    return entries_;
}