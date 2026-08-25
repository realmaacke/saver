#include "Project/Head.hpp"

#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

Head::Head() {
    if (!fs::exists(this->path_)) {
        this->set_branch("main");
    }
}

std::string Head::branch() const {
    std::ifstream in(path_, std::ios::binary);
    std::string name;
    std::getline(in, name);
    return name;
}

void Head::set_branch(const std::string& branch) {
    fs::create_directories(path_.parent_path());
    std::ofstream out(path_, std::ios::binary | std::ios::trunc);
    out << branch;
}