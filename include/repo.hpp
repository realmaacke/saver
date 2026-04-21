#pragma once
#include <filesystem>
#include <vector>

#include "sender.hpp"
#include "auth.hpp"

namespace fs = std::filesystem;

class Repository {
public:
    explicit Repository(fs::path base_path) 
        : base_path(base_path),
          saver_path(base_path / ".saver"),
          index_path(saver_path / "index"),
          sender() {};

    bool init();
    bool add(fs::path path_to_files);
    bool ignore(fs::path path_to_files);
    bool stage(fs::path path_to_files);

    // bool login(std::string ssh_key);

    int upload();

    int sendSingle();
    int sendMultiple();

    // Store/Load storage outside of memory
    bool loadIndex();
    bool saveIndex() const;


    // Kind of "getters" for Auth
    void auth_store_key(fs::path ssh_path) {
        auth.store_key(ssh_path);
    }

    void auth_use_key() {
        auth.use_key(this->sender);
    }

    void auth_upload_key() {
        auth.upload_key(this->sender);
    }

    private:
    fs::path base_path;
    fs::path saver_path;
    fs::path index_path;
    
    // used to store when recursive add
    std::vector<fs::path> add_storage;
    Sender sender;
    Auth auth;
};