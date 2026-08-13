#pragma once
#include <iostream>
#include <functional>

using cmd = std::function<int(int, char**)>;

class Command {
public:
    static cmd help();
    static cmd start();

    // static cmd init(SaverService& service);
    // static cmd status(SaverService& service);
    // static cmd add(SaverService& service);
    // static cmd describe(SaverService& service);
    // static cmd save(SaverService& service);
    // static cmd reset(SaverService& service);
};