#pragma once

#include "Project/Content/Describe.hpp"
#include "Project/Content/Tree.hpp"
#include <filesystem>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace fs = std::filesystem;

enum class ChangeType {
    Added, Removed, Modified
};

struct Change {
    std::string path;
    ChangeType type;
};

class Diff {
public:
    Diff(
        Describe* desc,
        Tree* tree,
        const fs::path& root,
        const fs::path& saver_root
    );
    std::vector<Change> resolve_diff(const std::string& hash);
    std::map<std::string, TreeNode> resolve_snapshot(const std::string& describe_hash);
private:
    fs::path root;
    fs::path saver_root;

    Describe* desc;
    Tree* tree;
};