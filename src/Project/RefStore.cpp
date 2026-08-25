#include "Project/RefStore.hpp"

#include <filesystem>
#include <fstream>
 
namespace fs = std::filesystem;

RefStore::RefStore() {
    fs::create_directories(refs_dir_);
}
 
fs::path RefStore::path_for(const std::string& branch) const {
    return refs_dir_ / branch;
}
 
void RefStore::set(const std::string& branch, const std::string& commit_hash) {
    std::ofstream out(path_for(branch), std::ios::binary | std::ios::trunc);
    out << commit_hash;
}
 
std::optional<std::string> RefStore::get(const std::string& branch) const {
    fs::path p = path_for(branch);
    if (!fs::exists(p)) {
        return std::nullopt;
    }
 
    std::ifstream in(p, std::ios::binary);
    std::string hash;
    std::getline(in, hash);
    return hash;
}
 
bool RefStore::exists(const std::string& branch) const {
    return fs::exists(path_for(branch));
}

std::vector<std::string> RefStore::list() const {
    std::vector<std::string> branches;
    for (const fs::directory_entry& entry : fs::directory_iterator(refs_dir_)) {
        if (entry.is_regular_file()) {
            branches.push_back(entry.path().filename().string());
        }
    }
    return branches;
}
