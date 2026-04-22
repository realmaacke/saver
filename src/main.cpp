#include <iostream>
#include <string>
#include <unordered_map>
#include <functional>
#include <curl/curl.h>

#include "saverService.hpp"

int main(int argc, char** argv) {
    SaverService saver(".");

    if (argc < 2) {
        std::cout << "saver --help";
        return 0;
    }

    return saver.executeCommand(argv[1], argc - 1, argv + 1);
}