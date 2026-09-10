#include <iostream>
#include <memory>
#include <string>
#include <filesystem>
#include <vector>

#include "Project/Project.hpp"
#include "IniStorage.hpp"
#include "Output/Output.hpp"
#include "Project/Cache.hpp"
#include "Project/Commit.hpp"
#include "Project/Object.hpp"
#include "Project/Tree.hpp"
#include "Service.hpp"
#include "Shipper/ProjectDTO.hpp"
#include "Shipper/UserDTO.hpp"

namespace fs = std::filesystem;

Project::Project()
{
    this->object = std::make_unique<Object>();
    this->object->create_obj_directory(this->root_dir);
    this->object->set_paths(this->root_dir);

    this->cache = std::make_unique<Cache>(this->object.get());
    this->tree = std::make_unique<Tree>(this->root_dir, this->object.get());
    this->commit = std::make_unique<Commit>(this->object.get());
    this->refs = std::make_unique<Refs>(this->root_dir);
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
    
    // if not signed in, then return;
    if (!Service::instance().user().alreadyConnectedUser()) {
        Output::error("You must be logged in to create a project");
        Output::print("Use: saver login <username> <password> | to login");
        return 1;
    }

    GetUserInfo::Response userInfo = Service::instance().send()
        .get<GetUserInfo::Response>(
            "auth/getUserInfo",
            true
    );

    if (!userInfo.success ||  (!std::is_integral_v<decltype(userInfo.userId)>)) {
        Output::error("Could not retrive correct user");
        return 1;
    }

    if (!fs::exists(proj_path)) {
        Output::error("Project path is invalid");
        return 1;
    }

    bool optional = (!optional_flag.empty() && optional_flag == "-f");

    if (!fs::is_empty(proj_path) && !optional) {
        Output::print("Directory is not empty.");
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
        Output::print("Could not create project.");
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
        Output::error("Root dir is empty");
        return 1;
    }
    return this->add_files_in_project(path);
    // what to do with the cache now?:
}

int Project::add_files_in_project(const std::string& path) {
    if (!fs::exists(path)) {
        Output::error("Invalid path to file");
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
        Output::error("You must be logged to upload to a project");
        Output::print("Use: saver login <username> <password> | to login");
        return 1;
    }

    GetUserInfo::Response userInfo = Service::instance().send()
        .get<GetUserInfo::Response>(
            "auth/getUserInfo",
            true
    );

    if (!userInfo.success ||  (!std::is_integral_v<decltype(userInfo.userId)>)) {
        Output::error("Could not retrive correct user");
        return 1;
    }

    const std::string current_commit = this->refs->get_current_commit();

    if (current_commit.empty()) {
        Output::error("You need to describe your changes in order to upload.");
        return 1;
    }

    // project info
    IniStorage storage(fs::path(fs::path(this->root_dir) / ".saver/proj.ini"));
    storage.loadStorage();
    std::string project_name = storage.getValue("project_name");

    // unfold commit + tree
    CommitObject commit_object = this->commit->unfold_commit_object(current_commit);
    TreeStructure tree = this->tree->unfold_tree(commit_object.tree);

    // passes commit + tree into dto
    std::vector<ObjectDTO> object_dto_array = this->object->array_obj_to_dto(tree.entries);

    ObjectDTO commit_dto;
    commit_dto.hash = current_commit;
    commit_dto.content = this->object->base64_encode(
        this->object->retrive_blob(current_commit)
    );
    object_dto_array.push_back(commit_dto);

    CommitToProject::Request body {current_commit, object_dto_array};

    CommitToProject::Response res = Service::instance().send()
        .post<CommitToProject::Response, CommitToProject::Request>(
            "proj/upload/" + userInfo.username + "/" + project_name,
            body,
            true
    );

    Output::print(res.message);
    return 0;
}