#include "global_context.hpp"
#include <iostream>
#include <vector>

Context& getContext() {
    static Context instance;
    return instance;
}

std::string buildApiEndpoint(
    Context& context,
    const ApiPath& path
) {
    std::string url = context.baseUrl;

    for (size_t i = 0; i < path.path.size(); i++) {
        if (path.path[i].empty()) continue;

        if (!url.empty() && url.back() != '/') {
            url += '/';
        }

        url += path.path[i];
    }

    return url;
}