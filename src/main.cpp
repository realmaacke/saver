#include <iostream>
#include "Core/Core.hpp"

/**
* Fill this out.
* */
int main(int argc, char** argv) { 
    Core saver_core;
    if (argc < 2) {
        std::cout << "Saver: unknown command" << std::endl;
        return 1;
    }
    
    return saver_core.execute(argv[1], argc - 1, argv + 1);
    // return saver.executeCommand(argv[1], argc - 1, argv + 1);
}