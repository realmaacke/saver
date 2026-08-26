#pragma once
#include <functional>

using cmd = std::function<int(int, char**)>;

class Command {
public:
    static cmd help();
    static cmd project();

    static cmd start();
    static cmd add();
    static cmd remove();
    static cmd reset();
    static cmd describe();
    static cmd upload();
    static cmd download();

    // Authentication
    static cmd login();
    static cmd disconnect();
};