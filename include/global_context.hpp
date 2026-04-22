#pragma once
#include <iostream>
#include <vector>


struct Context
{
    // change when deployed.
    std::string baseUrl = "http://localhost:9999/";
    
    // Somehow define and store theese someplace else.
    std::string username = "marcus";
    std::string project = "testProject";
};

Context& getContext();

struct ApiPath {
    std::vector<std::string> path;

    ApiPath(std::initializer_list<std::string> init)
    : path(init) {}
};

std::string buildApiEndpoint(Context& context, const ApiPath& path);