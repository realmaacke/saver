#pragma once
#include "Project/Content/Describe.hpp"
#include "Project/Content/Object.hpp"
#include "Project/Content/ObjectStore.hpp"
#include "Project/Content/Refs.hpp"
#include "Project/Content/Tree.hpp"
#include "Project/ProjectSettings.hpp"
#include <filesystem>
#include <memory>
#include <optional>

namespace  fs = std::filesystem;

class ProjectImpl {
public:
    ProjectImpl();
    int impl_help();
    int impl_start(const ProjectSettings& user_input);
    int impl_add(const fs::path& path);
    // cant be a path, needed for wildcard '*'
    int impl_remove(const std::string& path);

    int impl_describe(const std::string& message);
    int impl_upload(/* add user stuff here*/);

    std::optional<fs::path> find_project_root();

private:
    // std::unique_ptr<>
    std::unique_ptr<Object> obj;
    std::unique_ptr<ObjectStore> obj_store;
    std::unique_ptr<Tree> tree;
    std::unique_ptr<Describe> describe;
    std::unique_ptr<Refs> refs;
};