#pragma once
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

class Repository {
public:
    explicit Repository(fs::path base_path) 
        : base_path(base_path), saver_path(base_path / ".saver"),
            objects_path(saver_path / "objects"), refs_path(saver_path / "refs") {};
    bool init();
    bool add(fs::path path_to_files);
    bool ignore(fs::path path_to_files);

    // used to store when recursive add
    std::vector<fs::path> add_storage;
    void print_add_storage() const;
private:
    fs::path base_path;
    fs::path saver_path;

    // Used for something?
    fs::path objects_path;
    fs::path refs_path;

};