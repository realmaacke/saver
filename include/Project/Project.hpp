#pragma once
#include "Project/Object.hpp"
#include <string>
#include <filesystem>

struct project_info {
    std::string proj_name;
    std::string author;
};

class Project {
public: 
    Project();

    // Runs every start,
    // checks if proj is initialized
    void check_if_in_project();

    // Create a new project
    int create_new_project(
        const std::string& proj_path,
        const std::string& optional_flag = ""
    );

    project_info create_project_interface(
        const std::string& proj_path
    );
    //

private:
    std::string root_dir = "";
    Object object_;
};