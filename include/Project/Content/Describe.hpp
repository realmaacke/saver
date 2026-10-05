#pragma once
#include <optional>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

class Object;

struct DescribeObject {
    std::string tree;
    std::string message;
    std::optional<std::string> parent;
    std::string author;
};

class Describe {
public:
    Describe(
        Object* object,
        const fs::path& object_dir
    );

    std::string create_describe_object(
        const std::string& hash,
        const std::string& parent,
        const std::string& message,
        const std::string& author
    );

    DescribeObject decode_describe_object(const std::string& hash);

private:
    Object* object;
    fs::path object_dir;

    std::string append_ctx(const std::string& prefix, const std::string& ctx);
};