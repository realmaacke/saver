#pragma once
#include <unordered_map>
#include <functional>
#include <string>

#include "auth.hpp"
#include "repo.hpp"
#include "sender.hpp"



class SaverService {
public:
    SaverService(std::filesystem::path target_directory)
        : target_directory(std::move(target_directory)),
          sender(),
          auth(),
          repo(this->sender)
    {
        loadCommands();
        this->repo.init(this->target_directory);
    }

    void loadCommands();
    int executeCommand(const std::string& name, int argc, char** argv);

    Sender& getSender() { return sender; }
    Auth& getAuth() { return auth; }
    Repository& getRepository() { return repo; }
private:
    std::filesystem::path target_directory;
    
    Sender sender;
    Auth auth;
    Repository repo;

    std::unordered_map<std::string,
        std::function<int(int, char**)>> commands;
};