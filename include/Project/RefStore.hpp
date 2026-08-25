#pragma once
#include <filesystem>
#include <optional>
#include <vector>

namespace fs = std::filesystem;

class RefStore {
public:
    RefStore();

    void set(const std::string& branch, const std::string& commit_hash);
    std::optional<std::string> get(const std::string& branch) const;
    bool exists(const std::string& branch) const;
    std::vector<std::string> list() const;
 
private:
    fs::path path_for(const std::string& branch) const;
 
    fs::path refs_dir_ = ".saver/refs/heads";
};
