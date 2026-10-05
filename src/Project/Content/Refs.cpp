#include "Project/Content/Refs.hpp"
#include "Output/Codes/ErrorCodes.hpp"
#include "Output/Output.hpp"
#include <filesystem>
#include <fstream>
#include <string>



Refs::Refs(const fs::path& project_dir) {
    this->refs_directory = project_dir / ".saver" / "refs";
    this->HEAD_path = project_dir / ".saver" / "HEAD";
}

std::string Refs::get_current_describe() {
    std::string chapter = this->get_current_chapter();
    fs::path ref_path = this->chapter_ref_path(chapter);

    if (chapter.empty() ||!fs::exists(ref_path)) return {};

    std::ifstream ref_file(ref_path);

    if (!ref_file.is_open()) {
        Output::error(ErrorCode::FILE_ERROR, __FUNCTION__);
        return {};
    }

    std::string describe_hash;
    std::getline(ref_file, describe_hash);

    return describe_hash;
}


std::string Refs::get_current_chapter() {
    std::ifstream head_file(this->HEAD_path);
    
    if (!head_file.is_open()) {
        Output::error(ErrorCode::FILE_ERROR, __FUNCTION__);
        return {};
    }
    
    std::string line;
    std::getline(head_file, line);
    
    const std::string prefix = "ref: refs/heads/";

    if (line.rfind(prefix, 0) == 0) {
        return line.substr(prefix.size());
    }

    return {};
}

void Refs::update_current_describe(const std::string& describe_hash) {
    std::string chapter = this->get_current_chapter();
    fs::path ref_path = this->chapter_ref_path(chapter);

    fs::create_directories(ref_path.parent_path());

    std::ofstream ref_file(ref_path, std::ios::trunc);

    if (!ref_file.is_open()) {
        Output::error(ErrorCode::FILE_ERROR, __FUNCTION__);
        return;
    }

    ref_file << describe_hash;
    ref_file.close();
}

fs::path Refs::chapter_ref_path(const std::string& chapter) {
    return this->refs_directory / "heads" / chapter;
}