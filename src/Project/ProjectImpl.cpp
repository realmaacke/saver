#include <chrono>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <map>
#include <memory>
#include <optional>
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
#include "Sender/DTO/ProjectDTO.hpp"
#include "Service.hpp"
#include "User.hpp"
#include "nlohmann/json.hpp"

ProjectImpl::ProjectImpl() {
    std::optional<fs::path> root = this->find_project_root();
    if (!root) {
        return;
    }

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

    this->diffs = std::make_unique<Diff>(
        this->describe.get(),
        this->tree.get(),
        root.value(),
        root.value() / ".saver"
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
    const fs::path help_file = ResourceDir("help_instructions.txt");

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
        if (line.empty() || (line.rfind('#') == 0)) {
            continue;
        }

        const size_t line_end = line.rfind("<NL>");

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
    const fs::path root = fs::current_path() / ".saver";
    fs::create_directories(root);

    // creates objects & refs dir.
    fs::create_directories(root / "objects");

    const fs::path refs = root / "refs";
    const fs::path refs_heads = refs / "heads";
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

static bool is_ignored(const fs::path& path, fs::path dir) {
    fs::path pn = path.lexically_normal();
    dir = dir.lexically_normal();
    
    if (dir.filename().empty()) dir = dir.parent_path();
    auto [it, _] = 
        std::mismatch(dir.begin(), dir.end(), pn.begin(), pn.end());

    return it == dir.end();
}

int ProjectImpl::impl_add(const fs::path& path, const std::vector<fs::path>& ignored) {
    if (!fs::exists(path)){ 
        return 0;
    }

    for (const fs::path& dir : ignored) {
        if (is_ignored(path, dir)) return 0;
    }
    
    if (!fs::is_directory(path)) {
        // this->obj_store->add_to_store(path);
        this->obj_store->add_to_store(fs::relative(path, this->find_project_root().value()));
    }

    if (fs::is_directory(path)) {
        for (const fs::directory_entry& entry : fs::directory_iterator(path)) {
            if (fs::is_directory(entry)) {
                this->impl_add(entry.path(), ignored);
            } else {
                this->obj_store->add_to_store(fs::relative(entry.path(), this->find_project_root().value()));
            }
        }
    }
    return 0;
}

int ProjectImpl::impl_remove(const std::string& path) {
    const int del_res = this->obj_store->del_from_store(path);
    if (!del_res) {
        Output::info(InfoCode::INDEX_NO_DEL);
    }
    return del_res ? 0 : 1;
}

int ProjectImpl::impl_diff() {
    std::string chapter = this->refs->get_current_describe();
    std::map<std::string, TreeNode> snapshot = this->diffs->resolve_snapshot(chapter);
    
    for (std::pair<const std::basic_string<char>, TreeNode> & entry : snapshot) {
        Output::print(entry.first);
    }
    return 0;
}

int ProjectImpl::impl_describe(const std::string& message) {
    const std::string tree_hash = this->tree->describe_tree();
    const std::string parent_hash = this->refs->get_current_describe();
    const std::string describe_hash = this->describe->create_describe_object(
        tree_hash,
        parent_hash,
        message,
        "" // Username here
    );
    this->refs->update_current_describe(describe_hash);
    this->obj_store->wipe_index(); 
    return 0;
}

/*
*   Upload implementation method.
*   This method gets the relevant describes and traverses them, adding uploads till remote == cureent.
*   Upload::Start|Chunk|Done are structs that Upload:Lines uses (std::variant).
*/
int ProjectImpl::impl_upload() {    
    const fs::path root = fs::current_path() / ".saver";

    UserInfo user = Service::instance().usr().getUser();
    
    INIStorage storage(root / "project.ini");
    storage.loadStorage();

    std::string projectName = storage.getValue("project_name").value_or("undefined");


    if (projectName == "undefined") {
        Output::print("Error msg here impl_upload");
        return 1;
    }

    ProjectGetLatestHead::Response latestRes = Service::instance().send().get<ProjectGetLatestHead::Response>
    (
        "proj/" + user.username + "/" + projectName + "/latest-head",
        true
    );

    std::optional<std::string> remote_hash = std::nullopt;

    if (latestRes.data.is_string()) {
        remote_hash = latestRes.data.get<std::string>();
    }

    const std::string latest_describe = this->refs->get_current_describe();
    const std::string current_chapter = this->refs->get_current_chapter();

    if (latest_describe.empty()) { Output::info(InfoCode::NO_DESCRIBE_FOUND); return 0; }
    if (current_chapter.empty()) { Output::info(InfoCode::NO_CHAPTER_FOUND); return 0; }

    if (latest_describe == remote_hash) {
        Output::info(InfoCode::ALREADY_UPLOADED);
        return 0;
    }

    struct DescribeWithHash { DescribeObject desc; std::string hash;};
    std::vector<DescribeWithHash> describes;
    std::string cursor = latest_describe;

    // this vector will be sent with NDJOSN
    std::vector<Upload::Line> lines = {Upload::Start{"start", latest_describe, current_chapter}};

    while(!cursor.empty() && cursor != remote_hash) {
        DescribeWithHash desc_obj{this->describe->decode_describe_object(cursor), cursor};
        describes.push_back(desc_obj);

        if (!desc_obj.desc.parent.has_value()) {
            break;
        }
        cursor = desc_obj.desc.parent.value();
    }

    for (const DescribeWithHash& obj : describes) {
        const TreeStructure tree = this->tree->demolish_tree(obj.desc.tree);

        lines.emplace_back(Upload::Chunk{
            "chunk",
            obj.hash,
            std::nullopt,
            base_64_encode(
                this->obj->retrive_stored_blob(obj.hash)
            )
        });

        lines.emplace_back(Upload::Chunk{
            "chunk",
            obj.desc.tree,
            std::nullopt,
            base_64_encode(
                this->obj->retrive_stored_blob(obj.desc.tree)
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

    }

    lines.emplace_back(Upload::Done{"done"});

    Upload::Response res = Service::instance().send().postNDJSON<Upload::Response>
    (
        "proj/upload/" + user.username + "/" + projectName,
        lines,
        true
    );

    if (!res.success) {
        Output::print("Bad upload info add");
        return 1;
    }

    return 0;
}