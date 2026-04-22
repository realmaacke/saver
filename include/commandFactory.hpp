#include <iostream>
#include <functional>

#include "repo.hpp"

namespace fs = std::filesystem;

using CommandFn = std::function<int(int, char**)>;

class CommandFactory {
public:
    static CommandFn init(Repository& repo);
    static CommandFn status(Repository& repo);
    static CommandFn add(Repository& repo);
    static CommandFn describe(Repository& repo);
    static CommandFn save(Repository& repo);
    static CommandFn reset(Repository& repo);
};