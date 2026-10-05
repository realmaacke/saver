#include "Project/Content/Tree.hpp"
#include "Decoding.hpp"
#include "Output/Codes/ErrorCodes.hpp"
#include "Output/Output.hpp"
#include "Project/Content/Object.hpp"
#include "Project/Content/ObjectStore.hpp"
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

Tree::Tree(const fs::path& object_dir, Object* object, ObjectStore* object_store) {
    this->object_dir = object_dir;
    this->object = object;
    this->object_store = object_store;
}

std::string Tree::describe_tree() {
    TreeStructure tree = this->build_tree(this->object_store->get_index());
    return tree.hash;
}

TreeStructure Tree::build_tree(const std::vector<StoreEntry> nodes) {
    TreeStructure structure;
    std::map<std::string, std::vector<StoreEntry>> sub_directories;

    for (const StoreEntry& entry : nodes) {
        size_t slash = entry.path.find('/');

        // if entry in a sub directory
        if (slash != std::string::npos) {
            std::string folder = entry.path.substr(0, slash);
            std::string file = entry.path.substr(slash + 1);

            StoreEntry sub_entry = entry;
            sub_entry.path = file;

            sub_directories[folder].push_back(sub_entry);
            continue;
        }

        TreeNode treeNode;
        treeNode.local_path = entry.path;
        treeNode.perms = entry.mode;
        treeNode.hash = entry.hash;

        structure.entries.emplace_back(treeNode);
    }

    // itterate sub_directories
    for (const std::pair<const std::string, std::vector<StoreEntry>>& sub_dir : sub_directories) {
        const std::string& folder = sub_dir.first;
        const std::vector<StoreEntry> entries = sub_dir.second;

        TreeStructure subTree = this->build_tree(entries);

        TreeNode directory_entry;
        directory_entry.perms = "040000"; // permisions for directory
        directory_entry.local_path = folder;
        directory_entry.hash = subTree.hash;

        structure.entries.emplace_back(directory_entry);
    }

    // store the tree
    std::string ctx;

    for (const TreeNode& node : structure.entries) {
        ctx += node.perms + " " + node.local_path + " " + node.hash + "\n";
    }
    std::string taggedCTX = "tree " + std::to_string(ctx.size()) + '\0' + ctx;
    std::string hash = sha256(taggedCTX);
    this->object->blob_to_file(hash, taggedCTX);

    structure.hash = hash;
    return structure;
}

TreeStructure Tree::demolish_tree(const std::string& hash) {
    TreeStructure base_tree;
    base_tree.hash = hash;

    fs::path tree_path = this->object_dir / hash.substr(0, 2) / hash.substr(2);

    std::ifstream tree_file(tree_path, std::ios::in | std::ios::binary);

    if (!tree_file.is_open()) {
        Output::error(ErrorCode::FILE_ERROR, __FUNCTION__);
        return base_tree;
    }

    // Read file > buffer > string
    std::ostringstream buffer;
    buffer << tree_file.rdbuf();
    std::string raw = buffer.str();

    // all blobs contain null char
    size_t null_pos = raw.find('\0');

    if (null_pos == std::string::npos) {
        Output::error(ErrorCode::FILE_MALFORMED, __FUNCTION__);
        return base_tree;
    }

    std::istringstream ss(raw.substr(null_pos + 1));
    std::string line;

    while(std::getline(ss, line)) {
        if (line.empty()) continue;

        std::istringstream entry_ss(line);

        TreeNode node;

        entry_ss >> node.perms >> node.local_path >> node.hash;

        if (node.perms.empty() || node.hash.empty()) continue;

        // directory
        if (node.perms == "040000") {
            node.blob = this->object->retrive_stored_blob(node.hash);
            base_tree.entries.push_back(node);

            TreeStructure sub_tree = this->demolish_tree(node.hash);

            for (TreeNode sub_node : sub_tree.entries) {
                sub_node.local_path = node.local_path + "/" + sub_node.local_path;
                base_tree.entries.push_back(sub_node);
            }
            continue;
        }

        node.blob = this->object->retrive_stored_blob(node.hash);
        base_tree.entries.push_back(node);
    }

    return base_tree;
}