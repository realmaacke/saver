#pragma once
#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace fs = std::filesystem;

struct IndexNode {
    std::string permissions;
    std::string path;
    std::string hash;
};

class Object {
    public:
    // Create directory.
    void create_obj_directory(const fs::path& root_path);
    void create_index_file(const fs::path& root_path);

    // Runs on every add.
    void set_paths(const fs::path& path);
    
    // hash + byte + mode
    std::string sha256(const std::string& data);
    std::string transform_file(const fs::path& path);
    std::string get_permissions(const fs::path& path);


    // Index
    void update_index(
        const fs::path& path,
        const std::string& permissions,
        const std::string& hash
    );
    std::vector<IndexNode> get_index();
    void reset_index();


    // finds the object
    std::optional<fs::path> object_path(const std::string& hash);
    void store_object(const std::string& hash, const std::string& content);
    // Append files to the object/
    // return files from object/
private:
    fs::path obj_dir;
    fs::path index_path;
};