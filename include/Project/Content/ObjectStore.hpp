#pragma once
#include <string>
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

// forward declare.
class Object;

struct StoreEntry {
    std::string path;
    std::string hash;
    std::string mode;
};

class ObjectStore {
public:
    ObjectStore(Object* object, const fs::path& index);

    void add_to_store(const fs::path& path);
    bool del_from_store(const fs::path& path);
    std::vector<StoreEntry> get_index();

    // save to index here
    void store_to_index(
        const std::string& hash,
        const std::string& perms,
        const fs::path& local_path
    );
    
    void wipe_index();
private:
    std::vector<StoreEntry> store;
    Object* object;

    fs::path index_path;

};