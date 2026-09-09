#include "Project/Tree.hpp"
#include "Output/Output.hpp"
#include "Project/Object.hpp"
#include <filesystem>
#include <map>
#include <string>
#include <vector>

Tree::Tree(const fs::path& proj_path, Object* object) {
    this->tree_path = fs::path(proj_path / ".saver/trees");
    this->object = object;

    if (!fs::exists(this->tree_path.parent_path())) {
        fs::create_directories(this->tree_path);
    }
}

/*
* Method that builds a describe tree.
* Gets the files from the index
* 
 */
TreeStructure Tree::build_tree(const std::vector<IndexNode> nodes) {
    TreeStructure structure;
    std::map<std::string, std::vector<IndexNode>> grouped;

    for (const IndexNode& node : nodes) {
        size_t slash = node.path.find('/');

        if (slash != std::string::npos) {
            std::string folder = node.path.substr(0, slash);
            std::string file = node.path.substr(slash + 1);

            IndexNode sub_node = node;
            sub_node.path = file;

            grouped[folder].push_back(sub_node);
            continue;
        }

        TreeNode tree_node;

        tree_node.file = node.path;
        tree_node.permissions = node.permissions;
        tree_node.hash = node.hash;

        structure.entries.emplace_back(tree_node);
    }

    for (const std::pair<const std::string, std::vector<IndexNode>>& group : grouped) {
        const std::string& folder_name = group.first;
        const std::vector<IndexNode>& folder_nodes = group.second;

        TreeStructure subTree = this->build_tree(folder_nodes);

        TreeNode folder_entry;
        folder_entry.permissions = "040000";
        folder_entry.file = folder_name;
        folder_entry.hash = subTree.hash;

        structure.entries.emplace_back(folder_entry);
    }

    std::string content;
    for (const TreeNode& node : structure.entries) {
        content += node.permissions + " " + node.file + " " + node.hash + "\n";
    }
    std::string tagged = "tree " + std::to_string(content.size()) + '\0' + content;

    std::string hash = this->object->sha256(tagged);

    this->object->store_object(hash, tagged);
    
    structure.hash = hash;

    return structure;
}

std::string Tree::describe_tree(const std::string& msg) {
    TreeStructure tree = this->build_tree(this->object->get_index());
    return tree.hash;
}