#pragma once
#include <filesystem>

namespace fs = std::filesystem;

class Refs {
public:
    Refs(const fs::path& root_dir);
    std::string get_current_commit();
    void update_current_commit(const std::string& commit_hash);

private:
    fs::path ref_path; 
};