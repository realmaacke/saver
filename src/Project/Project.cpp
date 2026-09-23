#include <iostream>
#include <memory>
#include <string>
#include <filesystem>
#include <vector>

#include "Project/Project.hpp"
#include "IniStorage.hpp"
#include "Output/ErrorCode.hpp"
#include "Output/Output.hpp"
#include "Project/Cache.hpp"
#include "Project/Commit.hpp"
#include "Project/Object.hpp"
#include "Project/Tree.hpp"
#include "Service.hpp"
#include "Shipper/ProjectDTO.hpp"
#include "Shipper/UserDTO.hpp"
#include "curl/system.h"

namespace fs = std::filesystem;

Project::Project()
{
    this->object = std::make_unique<Object>();
    this->cache = std::make_unique<Cache>(this->object.get());
    this->commit = std::make_unique<Commit>(this->object.get());


    if (this->check_if_in_project()) {
        this->object->create_obj_directory(this->root_dir);
        this->object->set_paths(this->root_dir);
        this->tree = std::make_unique<Tree>(this->root_dir, this->object.get());
        this->refs = std::make_unique<Refs>(this->root_dir);
    }

}


/*
* Runs on every start.
* Called from main.cpp
*/
bool Project::check_if_in_project() {
    if (fs::exists(".saver/proj.ini")) {
        this->root_dir = ".";
        return true;
    }
    return false;
}

int Project::create_new_project(
    const std::string& proj_path,
    const std::string& optional_flag) {
    
    std::string name;

    if (fs::exists(fs::path(proj_path) / ".saver")) {
        Output::warning(ErrorType::ALREADY_IN_PROJECT);
        return 1;
    }
    
    // if not signed in, then return;
    if (!Service::instance().user().alreadyConnectedUser()) {
        Output::warning(ErrorType::AUTH_REQUIRED);
        Output::print("Syntax: saver login <username> <password>");
        return 1;
    }

    GetUserInfo::Response userInfo = Service::instance().send()
        .get<GetUserInfo::Response>(
            "auth/getUserInfo",
            true
    );

    if (!userInfo.success ||  (!std::is_integral_v<decltype(userInfo.userId)>)) {
        Output::warning(ErrorType::USER_RETRIVAL);
        return 1;
    }

    if (!fs::exists(proj_path)) {
        Output::warning(ErrorType::INVALID_PATH);
        return 1;
    }
    bool optional = (!optional_flag.empty() && optional_flag == "-f");

    if (!fs::is_empty(proj_path) && !optional) {
        Output::warning(ErrorType::DIRECTORY_NOT_EMPTY);
        Output::print("To force create an directory, add -f flag");
        return 1;
    }

    if (!fs::is_empty(proj_path) && optional) {
        Output::print("Creating project inside : " + proj_path);   
    } else {
        Output::print("Creating project");
    }
    
    // call init_project
    name = this->create_project_name(proj_path);

    CreateProject::Response project_result = Service::instance().send()
        .get<CreateProject::Response>(
            "proj/init/" + userInfo.username + "/" + name,
            true
    );

    if (!project_result.success) {
        Output::warning(ErrorType::COULD_NOT_CREATE_PROJ);
        Output::print("reason: " + project_result.message);
        return 1;
    }

    Output::print("Succesfully created " + name);
    
    this->create_saver_files(proj_path, name);
    return 0;
}

void Project::create_saver_files(
    const fs::path& proj_root,
    const std::string& name
) {
    IniStorage storage(proj_root / ".saver/proj.ini");
    storage.createStorage();
    storage.updateStorage("project_name", name);
    storage.saveStorage();

    // Creates index file here.
    
    this->object->create_index_file(proj_root);
}

const std::string Project::create_project_name(const std::string& proj_path) {
    std::string name;
    fs::path folderName(proj_path);

    std::cout << "Name of project (" << folderName.filename().string() << "): ";
    std::getline(std::cin, name);

    if (name.empty()) {
        return folderName.filename().string();
    }

    return name;
}

int Project::prepare_to_add_files(const std::string& path) {
    if (this->root_dir.empty()) {
        Output::warning(ErrorType::EMPTY_PROJECT);
        return 1;
    }
    return this->add_files_in_project(path);
    // what to do with the cache now?:
}

int Project::add_files_in_project(const std::string& path) {
    if (!fs::exists(path)) {
        return 1;
    }

    if (!fs::is_directory(path) && fs::is_regular_file(path)) {
        this->cache->add_to_cache(path);
    }

    if (fs::is_directory(path)) {
        for (const fs::directory_entry& entry : fs::directory_iterator(path)) {
            if (fs::is_directory(entry)) {
                this->add_files_in_project(entry.path());
            } else {
                this->cache->add_to_cache(entry.path());
            }
        }
    }
    return 0;
}

int Project::describe_cache(const std::string& message) {
    if (message.empty()) {
        Output::print("The describe cant be empty");
        return 0;
    }
    std::string tree_hash = this->tree->describe_tree(message);
    std::string parent_hash = this->refs->get_current_commit();
    std::string commit_hash = this->commit->create_commit_object(tree_hash, parent_hash, message, "");

    this->refs->update_current_commit(commit_hash);

    this->object->reset_index();

    return 0;
}

int Project::upload_commit() {
    // if not signed in, then return;
    if (!Service::instance().user().alreadyConnectedUser()) {
        Output::warning(ErrorType::AUTH_REQUIRED);
        Output::print("Syntax: saver login <username> <password>");
        return 1;
    }

    GetUserInfo::Response userInfo = Service::instance().send()
        .get<GetUserInfo::Response>(
            "auth/getUserInfo",
            true
    );

    if (!userInfo.success ||  (!std::is_integral_v<decltype(userInfo.userId)>)) {
        Output::warning(ErrorType::USER_RETRIVAL);
        return 1;
    }

    const std::string current_commit = this->refs->get_current_commit();
    const std::string current_chapter = this->refs->get_current_chapter();

    if (current_commit.empty()) {
        Output::warning(ErrorType::NO_DESCRIBE);
        return 1;
    }

    if (current_chapter.empty()) {
        Output::warning(ErrorType::DETERMINE_CHAPTER);
        return 1;
    }

    // project info
    IniStorage storage(fs::path(fs::path(this->root_dir) / ".saver/proj.ini"));
    storage.loadStorage();
    std::string project_name = storage.getValue("project_name");

    // unfold commit + tree
    CommitObject commit_object = this->commit->unfold_commit_object(current_commit);
    TreeStructure tree = this->tree->unfold_tree(commit_object.tree);

    std::vector<upload::Line> lines;

    lines.emplace_back(upload::Start{"start", current_commit, current_chapter});
    
    this->object->multiple_obj_to_chunks(
        lines,
        "chunk",
        tree.entries
    );

    lines.emplace_back(upload::Done{"done", true, "done"});

    upload::Response res = Service::instance().send()
        .postNDJSON<upload::Response>(
            "proj/upload/" + userInfo.username + "/" + project_name,
            lines,
            true,
            [](curl_off_t now, curl_off_t total) {
                Output::print_progress_bar(now, total);
            }
        );

    if (!res.success) {
        Output::warning(ErrorType::BAD_UPLOAD);
        return 1;
    }


    return 0;
}