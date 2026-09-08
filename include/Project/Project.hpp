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
    bool check_if_in_project();

    void create_saver_files(
        const fs::path& proj_path,
        const std::string& name
    );

    // Create a new project
    int create_new_project(
        const std::string& proj_path,
        const std::string& optional_flag = ""
    );

    const std::string create_project_name(
        const std::string& proj_path
    );

    int prepare_to_add_files(const std::string& file);
    int add_files_in_project(const std::string& file);
    int describe_cache(const std::string& message);

private:
    std::string root_dir = "";

    
    std::unique_ptr<Object> object;
    std::unique_ptr<Cache> cache;
};