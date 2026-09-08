#include <fstream>
#include <iostream>
#include <string>
#include <filesystem>
#include <type_traits>

#include "Project/Project.hpp"
#include "IniStorage.hpp"
#include "Output/Output.hpp"
#include "Service.hpp"
#include "Shipper/ProjectDTO.hpp"
#include "nlohmann/json.hpp"

namespace fs = std::filesystem;

Project::Project() {
    this->object_.create_obj_directory(this->root_dir); 
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

int Project::add_files_in_project(const std::string& path) {
    if (!fs::exists(path)) {
        Output::error("Invalid path to file");
        return 1;
    }

    if (!fs::is_directory(path) && fs::is_regular_file(path)) {
        this->cache_.add_to_cache(path);
    }

    if (fs::is_directory(path)) {
        for (const fs::directory_entry& entry : fs::directory_iterator(path)) {
            if (fs::is_directory(entry)) {
                this->add_files_in_project(entry.path());
            }
            this->cache_.add_to_cache(entry.path());
        }
    }

    // Output::print("DEBUG: file added");
    return 0;
}

int Project::describe_cache(const std::string& message) {
    if (message.empty()) {
        Output::print("The describe cant be empty");
        return 0;
    }

    // take all the blobs and stuff and create a tree.
    // here we will have to manage refs.
    // 

    return 0;
}