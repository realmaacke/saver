#include <iostream>
#include <string>
#include <filesystem>
#include "Project/Project.hpp"
#include "Output/Output.hpp"
#include "Project/Object.hpp"
#include "Service.hpp"
#include "Utility/Ini.hpp"

namespace fs = std::filesystem;

Project::Project() {
    
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
    
    // if not signed in, then return;
    if (!Service::instance().user().alreadyConnectedUser()) {
        Output::error("You must be logged in to create a project");
        Output::print("Use: saver login <username> <password> | to login");
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
        project_info proj_info = this->create_project_interface(proj_path);
        return 0;
    }

    Output::print("Creating project");

}

project_info Project::create_project_interface(const std::string& proj_path) {
    project_info proj_info;

    Output::print("Name of project:");
    std::cin >> proj_info.proj_name;

    return proj_info;
}