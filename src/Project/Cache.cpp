#include "Project/Cache.hpp"
#include "Output/Output.hpp"
#include "Storage.hpp"
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

Cache::Cache() {
    if(!fs::exists(".saver/cache")) {
        fs::create_directories(".saver/cache");
    }
}

void Cache::add(const std::filesystem::path& file) {
    if (!fs::exists(file)) {
        Output::error("File does not exist");
        return;
    }

    if (fs::is_directory(file)) {
        for (const fs::directory_entry& entry : fs::recursive_directory_iterator(file)) {
            if (entry.is_regular_file()) {
                this->add_file(
                    entry.path(),
                    fs::relative(entry.path(), file.parent_path()));
            }
        }
        return;
    }

    if (fs::is_regular_file(file)) {
        add_file(file, file.filename());
        return;
    }

    Output::error("File is not regular");
    return;
};

void Cache::add_file(const std::filesystem::path& file, const std::filesystem::path& relative_path) {
    fs::path dest = this->cache_dir / relative_path;
    fs::create_directories(dest.parent_path());
    fs::copy_file(file, dest, fs::copy_options::overwrite_existing);

    entries_.push_back(CacheEntry(file, dest));
}

const std::vector<CacheEntry>& Cache::entries() const {
    return entries_;
}

void Cache::clear() {
    entries_.clear();
}