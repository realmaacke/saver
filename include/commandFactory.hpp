#include <iostream>
#include <functional>

#include "repo.hpp"
#include "auth.hpp"
#include "saverService.hpp"

namespace fs = std::filesystem;

using CommandFn = std::function<int(int, char**)>;

class CommandFactory {
public:
    // CRUD actions and other stuff.
    static CommandFn init(SaverService& service);
    static CommandFn status(SaverService& service);
    static CommandFn add(SaverService& service);
    static CommandFn describe(SaverService& service);
    static CommandFn save(SaverService& service);
    static CommandFn reset(SaverService& service);

    // Auth actions
    static CommandFn auth(SaverService& service);
};