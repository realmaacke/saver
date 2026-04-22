#include <unordered_map>
#include <functional>
#include <string>

#include "repo.hpp"
#include "commandFactory.hpp"

using CommandFn = std::function<int(int, char**)>;

CommandFn CommandFactory::init(Repository& repo) {
    return [&repo](int, char**) {
        repo.initRepo();
    
        return 0;
    };
};

CommandFn CommandFactory::status(Repository& repo) {
    return [&repo](int, char**) {
        return repo.status() ? 0 : 1;
    };
};

CommandFn CommandFactory::add(Repository& repo) {
    return [&repo](int argc, char** argv) {
        if (argc < 2) {
            std::cout << "follow-up with the path to save" << std::endl;
            return 1;
        }
        return repo.add(argv[1]) ? 0 : 1;
    };
};

CommandFn CommandFactory::describe(Repository& repo) {
    return [&repo](int, char**) {
        return repo.describe() ? 0 : 1;
    };
};

CommandFn CommandFactory::save(Repository& repo) {
    return [&repo](int, char**) {
        return repo.save() ? 0 : 1;
    };
};

CommandFn CommandFactory::reset(Repository& repo) {
    return [&repo](int, char**) {
        return repo.reset() ? 0 : 1;
    };
};