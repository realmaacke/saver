#pragma once
#include <functional>

using cmd = std::function<int(int, char**)>;

class Commands {
public:
    static cmd help();
    static cmd project();

    static cmd start();
    static cmd add();
    static cmd remove();

    static cmd describe();
    static cmd upload();
    static cmd download();

    // Authentication
    static cmd connect();
    static cmd disconnect();
};