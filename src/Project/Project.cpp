#include <iostream>
#include <string>
#include <filesystem>
#include <type_traits>

#include "Project/Project.hpp"
#include "Output/Output.hpp"
#include "Service.hpp"
#include "Shipper/ProjectDTO.hpp"

namespace fs = std::filesystem;

Project::Project() {
    this->object_.create_obj_directory(this->root_dir); 
}

/*
* Runs on every start.
* Called from main.cpp
*/
void Project::check_if_in_project() {
    if (fs::exists(".saver/proj.ini")) {
        this->root_dir = ".";
        return;
    }
    return;
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
        Output::print("Could not retrive correct user, try disconnecting and the connecting again.");
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
    
    name = this->create_project_name(proj_path, userInfo.username);

    // call init_project

    nlohmann::json body;

    CreateProject::Response result = Service::instance().send()
        .post<CreateProject::Response>(
            "/proj/" + userInfo.username + "/" + name,
            body,
            true
    );
    

    if (!result.success) {
        Output::print("Could not create project.");
        Output::print("reason: " + result.message);
        return 1;
    }

    Output::print("Succesfully created " + name);
    return 0;
}

const std::string Project::create_project_name(const std::string& proj_path, const std::string& username) {
    std::string name;
    Output::print("Name of project (" + proj_path + "):");
    std::cin >> name;

    // Do tests here to ensure its a viable name.

    // Contact remote to check if user has same name.

 
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

    Output::print("DEBUG: file added");
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