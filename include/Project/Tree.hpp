#pragma once
#include "Project/Object.hpp"
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;


struct TreeNode {
    std::string permissions;
    std::string file;
    std::string hash;
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
private:
    fs::path tree_path;
    Object* object;
};
