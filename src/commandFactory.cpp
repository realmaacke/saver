#include <unordered_map>
#include <functional>
#include <string>

#include "repo.hpp"
#include "auth.hpp"
#include "commandFactory.hpp"

using CommandFn = std::function<int(int, char**)>;

CommandFn CommandFactory::init(SaverService& service) {
    return [&service](int, char**) {
        service.getRepository().initRepo();
    
        return 0;
    };
};

CommandFn CommandFactory::status(SaverService& service) {
    return [&service](int, char**) {
        return service.getRepository().status() ? 0 : 1;
    };
};

CommandFn CommandFactory::add(SaverService& service) {
    return [&service](int argc, char** argv) {
        if (argc < 2) {
            std::cout << "follow-up with the path to save" << std::endl;
            return 1;
        }
        return service.getRepository().add(argv[1]) ? 0 : 1;
    };
};

CommandFn CommandFactory::describe(SaverService& service) {
    return [&service](int, char**) {
        return service.getRepository().describe() ? 0 : 1;
    };
};

CommandFn CommandFactory::save(SaverService& service) {
    return [&service](int, char**) {
        return service.getRepository().save() ? 0 : 1;
    };
};

CommandFn CommandFactory::reset(SaverService& service) {
    return [&service](int, char**) {
        return service.getRepository().reset() ? 0 : 1;
    };
};

CommandFn CommandFactory::auth(SaverService& service) {
    return [&service](int argc, char** argv) {
        std::vector<std::string> raw(argv, argv + argc);
        std::vector<std::string> params(raw.begin() + 1, raw.end());

        if (params[0] == "upload") {
            return service.getAuth().upload_key(service.getSender());
        } else if (params[0] == "set" && params[1] == "key" && params.size() > 2) {
            return service.getAuth().store_key(params[2]);
        } else if (params[0] == "login") {
            return service.getAuth().login_user(service.getSender());
        }

        return 1;
    };
};