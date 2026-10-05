#include "Project/Content/Describe.hpp"
#include "Decoding.hpp"
#include "Output/Codes/ErrorCodes.hpp"
#include "Output/Output.hpp"
#include "Project/Content/Object.hpp"
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <string>

Describe::Describe(Object* object, const fs::path& object_dir) {
    this->object = object;
    this->object_dir = object_dir;
}

std::string Describe::create_describe_object(
    const std::string& hash,
    const std::string& parent,
    const std::string& message,
    const std::string& author
) {
    std::string ctx;

    ctx += this->append_ctx("tree", hash);

    if (!parent.empty()) ctx += this->append_ctx("parent", parent);

    ctx += this->append_ctx("message", message);
    ctx += this->append_ctx("author", author);

    std::string tagged_ctx = "describe "
        + std::to_string(ctx.size())
        + '\0'
        + ctx
    ;

    std::string ctx_hash = sha256(tagged_ctx);
    this->object->blob_to_file(ctx_hash, tagged_ctx);

    return ctx_hash;
}

std::string Describe::append_ctx(const std::string& prefix, const std::string& ctx) {
    return {prefix + " " + ctx + '\n'};
}

DescribeObject Describe::decode_describe_object(const std::string& hash) {
    fs::path describe_path = this->object_dir / hash.substr(0, 2) / hash.substr(2);
    DescribeObject desc_obj;

    if (!fs::exists(describe_path)) {
        Output::error(ErrorCode::INVALID_PATH, __FUNCTION__);
        return desc_obj;
    }

    std::ifstream desc_file(describe_path, std::ios::in);

    if (!desc_file.is_open()) {
        Output::error(ErrorCode::FILE_ERROR, __FUNCTION__);
        return desc_obj;
    }

    std::string line;
    while(std::getline(desc_file, line)) {
        size_t null_pos = line.find('\0');

        if (null_pos != std::string::npos) {
            size_t tree_pos = line.find("tree ", null_pos);
            if (tree_pos != std::string::npos) {
                desc_obj.tree = line.substr(tree_pos + 5);
            }
            continue;
        }

        if (line.rfind("parent ", 0) == 0) {
            desc_obj.parent = line.substr(7);
        } else if (line.rfind("message ", 0) == 0) {
            desc_obj.message = line.substr(8);
        } else if (line.rfind("author ", 0) == 0) {
            desc_obj.author = line.substr(7);
        }
    }
    return desc_obj;
}