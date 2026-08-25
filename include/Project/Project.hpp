#pragma once
#include "Project/Cache.hpp"
#include "Project/CacheStore.hpp"
#include "Project/Head.hpp"
#include "Project/RefStore.hpp"
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

    bool describe_cache(std::string& message);
    bool remove_from_cache(const std::string& path);

private:
    std::string root_dir;
    // Used with other commands.
    bool has_project;

    Cache cache_;
    CacheStore cacheStore_;
    RefStore refStore_;
    Head head_;
};