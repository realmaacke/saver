#include "Project/Refs.hpp"
#include <filesystem>
#include <fstream>
#include <string>

Refs::Refs(const fs::path& root_dir) {
    this->refs_dir = fs::path(root_dir / ".saver/refs");
    this->head_path = fs::path(root_dir / ".saver/HEAD");

    fs::create_directories(this->refs_dir / "heads");

    if (!fs::exists(this->head_path)) {
        std::ofstream head_file(this->head_path, std::ios::trunc);
        head_file << "ref: refs/heads/main";
    }
}

fs::path Refs::chapter_ref_path(const std::string& chapter) {
    return this->refs_dir / "heads" / chapter;
}

std::string Refs::get_current_chapter() {
    std::ifstream head_file(this->head_path);
    std::string line;
    std::getline(head_file, line);

    const std::string prefix = "ref: refs/heads/";

    if (line.rfind(prefix, 0) == 0) {
        return line.substr(prefix.size());
    }
    return "";
}

std::string Refs::get_current_commit() {
    std::string chapter = this->get_current_chapter();
    fs::path ref_path = this->chapter_ref_path(chapter);

    if (chapter.empty() || !fs::exists(ref_path)) {
        return "";
    }

    std::ifstream file(ref_path);
    std::string hash;
    std::getline(file, hash);
    return hash;
}

void Refs::update_current_commit(const std::string& commit_hash) {
    std::string chapter = this->get_current_chapter();
    fs::path ref_path = this->chapter_ref_path(chapter);

    fs::create_directories(ref_path.parent_path());

    std::ofstream ref_file(ref_path, std::ios::trunc);
    ref_file << commit_hash;
    ref_file.close();
}