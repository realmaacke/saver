#pragma once
#include <functional>
#include <string>
#include <unordered_map>
class Core {
public:
    Core();

    int execute(const std::string& name, int argc, char** argv);
private:
    void populate_cmd_list();


    std::unordered_map<std::string,
        std::function<int(int, char**)>>
        cmd_list;
};