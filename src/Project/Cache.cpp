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

bool Cache::add(const std::filesystem::path& file) {
    if (!fs::exists(file)) {
        Output::error("File does not exist");
        return false;
    }

    if (fs::is_directory(file)) {
        for (const fs::directory_entry& entry : fs::recursive_directory_iterator(file)) {
            if (entry.is_regular_file()) {
                
                this->add_file(
                    entry.path(),
                    fs::relative(fs::absolute(entry.path()), fs::absolute(file).parent_path())
                );
            }
        }
        return true;
    }

    if (fs::is_regular_file(file)) {
        add_file(file, file.filename());
        return true;
    }

    Output::error("File is not regular");
    return false;
};

void Cache::remove(const fs::path& path) {
    fs::path target = fs::absolute(path);
 
    auto is_under_target = [&](const CacheEntry& entry) {
        fs::path abs_original = fs::absolute(entry.original_path);
        fs::path rel = fs::relative(abs_original, target);
        // "." means exact match; anything not starting with ".." is inside target.
        return !rel.empty() && *rel.begin() != "..";
    };
 
    auto it = entries_.begin();
    while (it != entries_.end()) {
        if (is_under_target(*it)) {
            std::error_code ec;
            fs::remove(it->cached_path, ec); // best-effort; ignore if already gone
            it = entries_.erase(it);
        } else {
            ++it;
        }
    }
}


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