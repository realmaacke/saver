#pragma once
#include <filesystem>
#include <optional>
#include <vector>

namespace fs = std::filesystem;

class Object;
struct IndexNode;

struct TreeNode {
    std::string permissions;
    std::string file;
    std::string hash;
    std::optional<std::string> blob;
};

struct TreeStructure {
    std::vector<TreeNode> entries;
    std::string hash;
};

class Tree {
public:
    Tree(const fs::path& proj_path, Object* object);
    TreeStructure build_tree(const std::vector<IndexNode> nodes);

    std::string describe_tree(const std::string& msg);

    TreeStructure unfold_tree(const std::string& hash);
private:
    fs::path tree_path;
    Object* object;
};
