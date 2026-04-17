#include "repo.hpp"
#include <iostream>
#include <string>
#include <unordered_map>
#include <functional>
#include <curl/curl.h>

int main(int argc, char** argv) {
    Repository repo(".");

    repo.loadIndex();

    if (argc < 2) {
        std::cout << "saver --help";
        return 0;
    }

    std::unordered_map<std::string, std::function<int(int, char**)>> commands = {
        {
            "init",
            [&repo](int, char**) {
                return repo.init() ? 0 : 1;
            }
        },
        {
            "add",
            [&repo](int argc, char** argv) {

                if (argc < 3) {
                    std::cerr << "add command needs a path to file/files" << std::endl;
                    return 1;
                }

                std::filesystem::path files = argv[2];

                int run = repo.stage(files) ? 0 : 1;

                // repo.print_add_storage();

                return run;
            }
        },
        {
            "upload",
            [&repo](int, char**) {
                return repo.upload();
            }
        },
    };

    std::string execute = argv[1];

    auto it = commands.find(execute);

    if (it == commands.end()) {
        std::cerr << execute << " is not an recognizeable command, do: saver --help" << "\n";
        return 1;
    }
    return it->second(argc, argv);
}