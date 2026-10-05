#pragma once
#include "Project/Content/ObjectStore.hpp"
#include <filesystem>
#include <optional>
#include <vector>

namespace fs = std::filesystem;

class Object;
class ObjectStore;
class StoreEntry;

struct TreeNode {
    std::string perms;
    std::string local_path;
    std::string hash;
    std::optional<std::string> blob;
};

struct TreeStructure {
    std::vector<TreeNode> entries;
    std::string hash;
};

class Tree {
public:
    Tree(const fs::path& object_dir, Object* object, ObjectStore* object_store);
    
    std::string describe_tree();
    TreeStructure demolish_tree(const std::string& hash);

private:
    fs::path object_dir;
    Object* object;
    ObjectStore* object_store;
    
    TreeStructure build_tree(const std::vector<StoreEntry> nodes);
};