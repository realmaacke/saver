#pragma once
#include <filesystem>

namespace fs = std::filesystem;

class Refs {
public:
    Refs(const fs::path& project_dir);

    std::string get_current_describe();
    std::string get_current_chapter();
    
    void update_current_describe(const std::string& describe_hash);
private:
    fs::path refs_directory;
    fs::path HEAD_path;

    fs::path chapter_ref_path(const std::string& chapter);
};