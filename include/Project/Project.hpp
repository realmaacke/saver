#pragma once
#include <string>
#include "Project/ProjectImpl.hpp"
#include "Project/ProjectSettings.hpp"

class Project {
public:
    Project(); 
    bool inside_project();
    void project_environment();
    void system_environment();

    // callable methods.
    int project_help();
    int project_status();
    int project_start();
    int project_diff();
    
    int project_add(const std::string& path);
    int project_removal(const std::string& path);
    int project_describe(const std::string& message);
    int project_upload();
    
    ProjectSettings get_project_info();
private:
    ProjectImpl p_impl;
    std::string project_directory;

    void set_project_directory(const std::string& path);
    // Private prompts
};