#include "Project/Refs.hpp"
#include <filesystem>
#include <fstream>
#include <string>

Refs::Refs(const fs::path& root_dir) {
    this->ref_path = fs::path(root_dir / ".saver/refs/HEAD");
    fs::create_directories(this->ref_path.parent_path());

}

std::string Refs::get_current_commit() {
    if (!fs::exists(this->ref_path)) {
        return "";
    }

    std::ifstream file(this->ref_path);
    std::string hash;
    std::getline(file, hash);
    return hash;
}

void Refs::update_current_commit(const std::string& commit_hash) {
    fs::create_directories(this->ref_path.parent_path());

    std::ofstream ref_file(this->ref_path, std::ios::trunc);
    ref_file << commit_hash;
    ref_file.close();
}