#pragma once
#include <string>
#include <filesystem>

class Project {
public: 
    Project();

    int customize_project();
    void set_root(const std::string& path);
    void has_root();

    void create_project(std::string path);
    void createProjectFiles();

    bool populate_cache(const std::string& path);
    bool reset_cache();

private:
    std::string root_dir;
    // Used with other commands.
    bool has_project;
};