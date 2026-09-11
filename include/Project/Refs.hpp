#pragma once
#include <filesystem>

namespace fs = std::filesystem;

class Refs {
public:
    Refs(const fs::path& root_dir);
    std::string get_current_commit();
    void update_current_commit(const std::string& commit_hash);
    std::string get_current_chapter();

private:
    fs::path refs_dir;
    fs::path head_path;

    fs::path chapter_ref_path(const std::string& chapter);
};