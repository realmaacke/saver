#pragma once
#include "Project/Cache.hpp"
#include "Project/Object.hpp"
#include <string>
#include <filesystem>


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

    const std::string create_project_name(
        const std::string& proj_path,
        const std::string& username
    );

    int add_files_in_project(const std::string& file);
    int describe_cache(const std::string& message);

private:
    std::string root_dir = "";

    
    Cache cache_;
    Object object_;
};