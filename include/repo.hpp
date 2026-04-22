#pragma once
#include <filesystem>
#include <vector>

#include "sender.hpp"
#include "auth.hpp"

namespace fs = std::filesystem;

class Repository {
public:
    Repository(Sender& sender)
        : sender(sender) {};

void init(fs::path target_directory);
void initRepo();

// Actions
bool status();
bool add(fs::path path_to_add);
bool describe();
bool save();
bool reset();

// Underlying Actions
bool addToIndex(fs::path path_to_store);
std::vector<fs::path> loadFromIndex();

// Ignores
std::vector<fs::path> readIgnore() const;
bool executeIgnore(fs::path ignored_path);

private:
    Sender& sender;

    fs::path target_directory;
    fs::path saver_path;
    fs::path index_path;
    fs::path saver_ignore;

    bool hasCommited = false;
};