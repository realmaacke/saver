#include "Project/Tree.hpp"
#include "Output/Output.hpp"
#include "Project/Object.hpp"
#include <filesystem>
#include <fstream>
#include <map>
#include <sstream>
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


TreeStructure Tree::unfold_tree(const std::string& hash) {
    TreeStructure base_tree;
    base_tree.hash = hash;

    fs::path object_path = 
    this->object->retrive_obj_dir() / hash.substr(0, 2) / hash.substr(2);
    std::ifstream obj_file(object_path, std::ios::in | std::ios::binary);

    if (!obj_file.is_open()) {
        Output::error("unfold_tree(), could not open file");
        return base_tree;
    }

    std::ostringstream buffer;
    buffer << obj_file.rdbuf();
    std::string raw = buffer.str();

    size_t null_pos = raw.find('\0');
    if (null_pos == std::string::npos) {
        Output::error("unfold_tree(), malformed tree: no header separator");
        return base_tree;
    }

    std::istringstream ss(raw.substr(null_pos +1));
    std::string line;

    while(std::getline(ss, line)) {
        if (line.empty()) continue;

        std::istringstream entry_ss(line);
        TreeNode node;

        entry_ss >> node.permissions >> node.file >> node.hash;

        if (node.permissions.empty() || node.hash.empty()) {
            continue;
        }

        if (node.permissions == "040000") {
            node.blob = this->object->retrive_blob(node.hash);
            base_tree.entries.push_back(node);

            TreeStructure sub_tree = this->unfold_tree(node.hash);

            for (TreeNode sub_node : sub_tree.entries) {
                sub_node.file = node.file + "/" + sub_node.file;
                base_tree.entries.push_back(sub_node);
            }
            continue;
        }
        node.blob = this->object->retrive_blob(node.hash);
        base_tree.entries.push_back(node);
    }

    return base_tree;
}