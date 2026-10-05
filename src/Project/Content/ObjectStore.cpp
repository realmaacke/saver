#include "Project/Content/ObjectStore.hpp"
#include "Output/Codes/ErrorCodes.hpp"
#include "Output/Output.hpp"
#include "Project/Content/Object.hpp"
#include "Decoding.hpp"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

ObjectStore::ObjectStore(Object* object, const fs::path& index) {
    if (!fs::exists(index)) {
        Output::error(ErrorCode::INVALID_PATH, __FUNCTION__);
        return;
    }
    
    this->object = object;
    this->index_path = index;
}

void ObjectStore::add_to_store(const fs::path& path) {
    this->store = this->get_index();
    for (const StoreEntry& entry : this->store) {
        if (path == entry.path) return;
    }

    std::string bytes = this->object->file_to_blob(path, "blob", std::ios::binary);
    std::string hash = sha256(bytes);
    std::string mode = this->object->file_permissions(path);

    // store object
    this->object->blob_to_file(hash, bytes);
    // store index
    this->store_to_index(hash, mode, path);
}

bool ObjectStore::del_from_store(const fs::path& path) {
    std::vector<StoreEntry> entries = this->get_index();
    this->wipe_index();

    bool has_skipped = false;
    for (const StoreEntry& entry : entries) {
        if (entry.path == path) {
            has_skipped = true;
            continue;
        }

        this->store_to_index(entry.hash, entry.mode, entry.path);
    }

    return has_skipped;
}

void ObjectStore::store_to_index(
    const std::string& hash,
    const std::string& perms,
    const fs::path& local_path
) {
    std::ofstream out(this->index_path, std::ios::app);
    if (!out.is_open()) {
        Output::error(ErrorCode::FILE_ERROR, __FUNCTION__);
        return;
    }
    out << perms << " " << local_path.generic_string() << " " << hash << "\n"; 
}

void ObjectStore::wipe_index() {
    std::ofstream idx(this->index_path, std::ofstream::trunc);
    if (!idx.is_open()) {
        Output::error(ErrorCode::FILE_ERROR, __FUNCTION__);
        return;
    }
    idx << "";
    idx.close();
}

std::vector<StoreEntry> ObjectStore::get_index() {
    std::vector<StoreEntry> entries;

    std::ifstream idx(this->index_path);
    if (!idx.is_open()) {
        Output::error(ErrorCode::FILE_ERROR, __FUNCTION__);
        return entries;
    }
    std::string line;

    while(std::getline(idx, line)) {
        std::istringstream iss(line);
        StoreEntry entry;

        iss >> entry.mode >> entry.path >> entry.hash;
        entries.emplace_back(entry);
    }
    return entries;
}