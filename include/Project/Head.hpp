#pragma once

#include <filesystem>
#include <string>

class Head {
public:
    Head();

    std::string branch() const;
    void set_branch(const std::string& branch);

private:
    std::filesystem::path path_ = ".saver/HEAD";
};