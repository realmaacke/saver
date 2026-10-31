#include "Project/Content/Describe.hpp"
#include "Project/Content/Diff.hpp"
#include "Project/Content/Tree.hpp"
#include <map>
#include <memory>
#include <string>


Diff::Diff(Describe* desc, Tree* tree, const fs::path& root, const fs::path& saver_root) {
    this->desc = desc;
    this->tree = tree;

    this->root = root;
    this->saver_root = saver_root;
}

std::map<std::string, TreeNode> Diff::resolve_snapshot(const std::string& describe_hash) {
    std::map<std::string, TreeNode> files;
    std::string cursor = describe_hash;

    while (!cursor.empty()) {
        DescribeObject d = this->desc->decode_describe_object(cursor);

        for (const TreeNode& node : this->tree->demolish_tree(d.tree).entries) {
            if (node.perms == "040000") continue;       // files only
            files.try_emplace(node.local_path, node);   // newest seen first, so don't overwrite
        }

        if (!d.parent.has_value()) break;
        cursor = d.parent.value();
    }
    return files;
}