#include <chrono>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <map>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

#include "Decoding.hpp"
#include "Environment.hpp"
#include "Output/Output.hpp"
#include "Output/Formater.hpp"
#include "Output/Codes/ErrorCodes.hpp"
#include "Output/Codes/InformationCodes.hpp"

#include "Project/ProjectImpl.hpp"
#include "Project/ProjectSettings.hpp"

#include "Project/Content/Describe.hpp"
#include "Project/Content/ObjectStore.hpp"
#include "Project/Content/Refs.hpp"
#include "Project/Content/Tree.hpp"

#include "FileHandler.hpp"
#include "INIStorage.hpp"

#include "Sender/DTO/NDJSON.hpp"

ProjectImpl::ProjectImpl() {
    std::optional<fs::path> root = this->find_project_root();
    if (!root) return;

    this->obj = std::make_unique<Object>(
        root.value() / ".saver" / "objects"
    );
    this->obj_store = std::make_unique<ObjectStore>(
        this->obj.get(),
        root.value() / ".saver" / "index"
    );

    this->tree = std::make_unique<Tree>(
        root.value() / ".saver" / "objects",
        this->obj.get(),
        this->obj_store.get()
    );

    this->describe = std::make_unique<Describe>(
        this->obj.get(),
        root.value() / ".saver" / "objects"
    );

    this->refs = std::make_unique<Refs>(
        root.value()
    );
}

std::optional<fs::path> ProjectImpl::find_project_root() {
    fs::path current = fs::current_path();

    while (true) {
        if (fs::exists(current / ".saver")) {
            return current;
        }
        if (current == current.parent_path()) {
            return std::nullopt;
        }
        current = current.parent_path();
    }
}

int ProjectImpl::impl_help() {
    fs::path help_file = ResourceDir("help_instructions.txt");

    if (!fs::exists(help_file)) {
        Output::error(ErrorCode::INTERNAL_FILE_ERROR);
        return 1;
    }

    std::string line;
    std::ifstream file(help_file);

    if (!file.is_open()) {
        Output::error(ErrorCode::FILE_ERROR);
        return 1;
    }

    while(std::getline(file, line)) {
        if (line.empty()) continue;
        if(line.rfind("#") == 0) continue;

        size_t line_end = line.rfind("<NL>");

        if (line_end != std::string::npos) {
            Output::print(line.substr(0, line_end) + "\n");
            continue;
        }
        Output::print(line);
    }

    return 0;
}

int ProjectImpl::impl_start(const ProjectSettings& user_input) {
    ProjectSettings settings = user_input;

    settings.start_date = std::chrono::system_clock::now();
    settings.last_sync = settings.start_date;

    // creates root dir
    fs::path root = fs::current_path() / ".saver";
    fs::create_directories(root);

    // creates objects & refs dir.
    fs::create_directories(root / "objects");

    fs::path refs = root / "refs";
    fs::path refs_heads = refs / "heads";
    fs::create_directories(refs_heads);
    fs::create_directories(refs / "tags");

    if (!write_file(root / "index", "")) {
        Output::error(ErrorCode::FILE_ERROR, __FUNCTION__);
        return 1;
    }

    if (!write_file(refs_heads / settings.default_chapter, "")) {
        Output::error(ErrorCode::FILE_ERROR, __FUNCTION__);
        return 1;
    }

    if (!write_file(root / "HEAD", "ref: refs/heads/" + settings.default_chapter)) {
        Output::error(ErrorCode::FILE_ERROR, __FUNCTION__);
        return 1;
    }
    INIStorage storage(root / "project.ini");

    storage.createStorage();
    storage.updateMultipleValues(std::map<std::string, std::string> {
        {"project_name", settings.project_name},
        {"project_remote", settings.project_remote},
        {"default_chapter", settings.default_chapter},
        {"description", settings.description.value_or("")},
        {"is_private", settings.is_private ? "true" : "false"},
        {"start_date", time_point_to_str(settings.start_date)},
        {"last_sync", time_point_to_str(settings.last_sync)}
    });
    storage.saveStorage();

    return 0;
}

int ProjectImpl::impl_add(const fs::path& path) {
    if (!fs::exists(path)) return 0;

    if (!fs::is_directory(path)) {
        this->obj_store->add_to_store(path);
    }

    if (fs::is_directory(path)) {
        for (const fs::directory_entry& entry : fs::directory_iterator(path)) {
            if (fs::is_directory(entry)) {
                this->impl_add(entry.path());
            } else {
                this->obj_store->add_to_store(entry.path());
            }
        }
    }
    return 0;
}

int ProjectImpl::impl_remove(const std::string& path) {
    int del_res = this->obj_store->del_from_store(path);
    
    if (!del_res) Output::info(InfoCode::INDEX_NO_DEL);
    
    // reverse it, true == 1
    return !del_res;
}

int ProjectImpl::impl_describe(const std::string& message) {
    std::string tree_hash = this->tree->describe_tree();
    std::string parent_hash = this->refs->get_current_describe();
    std::string describe_hash = this->describe->create_describe_object(
        tree_hash,
        parent_hash,
        message,
        "" // Username here
    );
    this->refs->update_current_describe(describe_hash);
    this->obj_store->wipe_index(); 
    return 0;
}

int ProjectImpl::impl_upload() {
    std::optional<std::string> remote_hash = std::nullopt;
    std::string latest_describe = this->refs->get_current_describe();
    std::string current_chapter = this->refs->get_current_chapter();

    if (latest_describe.empty()) { Output::info(InfoCode::NO_DESCRIBE_FOUND); return 0; }
    if (current_chapter.empty()) { Output::info(InfoCode::NO_CHAPTER_FOUND); return 0; }

    struct DescribeWithHash { DescribeObject desc; std::string hash;};
    std::vector<DescribeWithHash> describes;
    std::string cursor = latest_describe;

    // this vector will be sent with NDJOSN
    std::vector<Upload::Line> lines;

    // add start
    lines.emplace_back(Upload::Start{"start", latest_describe, current_chapter});

    // add describe
    lines.emplace_back(Upload::Chunk{
        "chunk",
        latest_describe,
        std::nullopt,
        base_64_encode(
            this->obj->retrive_stored_blob(latest_describe)
        ),
    });

    while(!cursor.empty() && cursor != remote_hash) {
        DescribeWithHash desc_obj{this->describe->decode_describe_object(cursor), cursor};
        describes.push_back(desc_obj);

        if (!desc_obj.desc.parent.has_value()) break;
        cursor = desc_obj.desc.parent.value();
    }

    for (const DescribeWithHash& obj : describes) {
        TreeStructure tree = this->tree->demolish_tree(obj.desc.tree);

        lines.emplace_back(Upload::Chunk{
            "chunk",
            obj.hash,
            std::nullopt,
            base_64_encode(
                this->obj->retrive_stored_blob(obj.hash)
            )
        });

        for (const TreeNode& node : tree.entries) {
            lines.emplace_back(Upload::Chunk{
                "chunk",
                node.hash,
                node.perms,
                node.blob ? base_64_encode(*node.blob) : ""
            });
        }

        lines.emplace_back(Upload::Done{"done"});

        // Send lines here:
    }
    return 0;
}