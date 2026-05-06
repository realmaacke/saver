#include "repo.hpp"
#include <filesystem>
#include <iostream>
#include <fstream>

#include <openssl/sha.h>
#include <sstream>
#include <iomanip>

#include <json.hpp>

using json = nlohmann::json;
namespace fs = std::filesystem;

namespace Color
{
    constexpr const char *reset = "\033[0m";
    constexpr const char *red = "\033[31m";
    constexpr const char *green = "\033[32m";
    constexpr const char *yellow = "\033[33m";
    constexpr const char *blue = "\033[34m";
}

void Repository::init(fs::path target_directory)
{
    this->target_directory = target_directory;
    this->saver_path = target_directory / ".saver";
    this->index_path = this->saver_path / "index";
    this->saver_ignore = target_directory / ".saverIgnore";
    return;
};

bool repoExists(fs::path saver_path)
{
    if (fs::exists(saver_path))
    {
        return true;
    }
    std::cout << "fatal: Saver repository not found, use command: saver init" << std::endl;
    return false;
}

void Repository::initRepo()
{
    // Creates folder
    if (!fs::exists(this->saver_path))
    {
        fs::create_directories(this->saver_path);
    }

    // Stream creates the index file
    if (!fs::exists(this->index_path))
    {
        std::ofstream out(this->index_path);
        out << "[]";
    }

    return;
}

bool Repository::status()
{
    if (!repoExists(this->saver_path))
    {
        return false;
    }

    std::vector<IndexType> commited_files = this->loadFromIndex();

    std::cout << "Changes to be saved:" << std::endl;
    std::cout << "(use \"saver reset\" to unsave)" << std::endl;
    for (const auto &commit : commited_files)
    {
        std::cout << '\t'
                  << Color::blue
                  << commit.status
                  << ": "
                  << commit.name
                  << Color::reset
                  << std::endl;
    }
    return true;
}

/**
 * Recursive method that adds items to index.
 */
bool Repository::add(fs::path path_to_add)
{
    fs::path output_path = this->target_directory / path_to_add;

    if (!repoExists(this->saver_path))
    {
        return false;
    }

    // if the path itself is ignored, do nothing.
    if (this->executeIgnore(path_to_add))
    {
        return true;
    }

    if (!fs::exists(path_to_add))
    {
        std::cout << "saver: " << output_path << "did not match any files" << std::endl;
        return false;
    }

    if (fs::is_regular_file(path_to_add))
    {
        return this->addToIndex(path_to_add);
    }

    if (fs::is_directory(path_to_add))
    {
        for (const auto &entry : fs::directory_iterator(path_to_add))
        {
            if (!this->add(entry.path()))
            {
                return false;
            }
        }
        return true;
    }
    return true;
};

bool Repository::describe(std::string message)
{
    if (!repoExists(this->saver_path))
    {
        return false;
    }

    /**
     * Add the commit stuff here.
     */

    return true;
};

bool Repository::save()
{
    if (!repoExists(this->saver_path))
    {
        return false;
    }

    if (!this->hasCommited)
    {
        std::cout << "All is saved, try to describe if you got a new save!" << std::endl;
        return true;
    }
    return true;
};

bool Repository::reset()
{
    if (!repoExists(this->saver_path))
    {
        return false;
    }

    std::ofstream out(this->index_path);

    if (!out)
    {
        std::cerr << "fatal: saver could not reset the changes" << std::endl;
        return false;
    }

    out << "";
    return true;
};

bool Repository::addToIndex(fs::path path_to_store)
{

    std::ifstream in(this->index_path);
    std::vector<IndexType> current_index = this->loadFromIndex();

    json j;

    if (in)
    {
        in >> j;
    }
    std::string hash = this->hash_file(path_to_store);
    std::string status = "added";

    for (const auto &index : current_index)
    {
        if (path_to_store.generic_string() == index.name && index.hash != hash)
        {
            status = "modified";
            break;
        }
    }

    json entry = {
        {"name", path_to_store.generic_string()},
        {"hash", hash},
        {"status", status}};

    bool updatedFile = false;
    // prevent duplicates (simple version)
    for (auto &e : j)
    {
        if (e["name"] == entry["name"])
        {
            e["hash"] = hash;
            e["status"] = status;
            updatedFile = true;
            break;
        }
    }

    if (!updatedFile)
    {
        j.push_back(entry);
    }

    std::ofstream out(this->index_path);
    out << j.dump(4); // pretty print

    return true;
};

/**
 * method that loads the stored changes and returns them.
 */
std::vector<IndexType> Repository::loadFromIndex()
{
    std::ifstream in(this->index_path);
    std::vector<IndexType> result;

    if (!in)
    {
        std::cerr << "fatal: saver could not load the changes" << std::endl;
        return result;
    }

    json j;

    try
    {
        in >> j;
    }
    catch (const json::parse_error &e)
    {
        std::cout << "fatal: saver file is malformed, error: " << e.what() << std::endl;
    }

    for (const auto &item : j)
    {
        result.emplace_back(
            item["name"].get<fs::path>(),
            item["hash"].get<std::string>(),
            item["status"].get<std::string>());
    }
    return result;
};

/**
 * method that reads the ignore files and appends to an array.
 * also returns the array :)
 */
std::vector<fs::path> Repository::readIgnore() const
{
    std::vector<fs::path> ignore_rules;

    if (!fs::exists(this->saver_ignore))
        return ignore_rules;

    std::ifstream file(this->saver_ignore);

    std::string line;
    while (std::getline(file, line))
    {
        if (line.empty())
        {
            continue;
        }
        ignore_rules.emplace_back(line);
    }

    return ignore_rules;
};

/**
 * Function that gets all the rules in array.
 * Itterates that array and check if the path matches the ignored one.
 */
bool Repository::executeIgnore(fs::path ignored_path)
{
    fs::path sanitizedPath = fs::absolute(ignored_path).lexically_normal();

    std::vector<fs::path> ignore_rules = readIgnore();

    for (const auto &rule : ignore_rules)
    {
        fs::path sanitizedRule = fs::absolute(this->target_directory / rule).lexically_normal();

        auto missMatch = std::mismatch(
            sanitizedRule.begin(), sanitizedRule.end(),
            sanitizedPath.begin(), sanitizedPath.end());

        if (missMatch.first == sanitizedRule.end())
        {
            return true;
        }
    }
    return false;
}

std::string Repository::hash_file(fs::path file_path)
{
    std::ifstream file(file_path, std::ios::binary);

    if (!file)
        return "";

    SHA256_CTX ctx;
    SHA256_Init(&ctx);

    char buffer[8192];

    while (file.good())
    {
        file.read(buffer, sizeof(buffer));
        SHA256_Update(&ctx, buffer, file.gcount());
    }

    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_Final(hash, &ctx);

    std::ostringstream result;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i)
    {
        result << std::hex << std::setw(2) << std::setfill('0')
               << (int)hash[i];
    }

    return result.str();
}