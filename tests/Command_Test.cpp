#include <fstream>
#include <gtest/gtest.h>
#include <filesystem>
#include <initializer_list>
#include <string>
#include <vector>

#include "Commands.hpp"
#include "Environment.hpp"
#include "tests/mocks/Environment.hpp"


namespace fs = std::filesystem;

static inline int run_command(const cmd& c, std::initializer_list<const char*> args) {
    std::vector<std::string> arg_buffers(args.begin(), args.end());
    std::vector<char*> argv;
    argv.reserve(arg_buffers.size() + 1);

    for (std::basic_string<char>& arg : arg_buffers) {
        argv.push_back(arg.data());
    }
    argv.push_back(nullptr);
    return c(static_cast<int>(arg_buffers.size()), argv.data());
}

namespace commands_help {
    TEST(HelpTest, ReturnsFileMissing) {
        MockResourcePath() = "/does_not_exist.txt";
        EXPECT_EQ(run_command(Commands::help(), {"saver", "help"}), 1);
    }
    TEST(HelpTest, ReturnsZeroWhenFileOpens) {
        fs::path const file = fs::temp_directory_path() / "saver_help.txt";
        std::ofstream(file) << "# c\nussage: saver<NL>\n";
        MockResourcePath() = file;
        EXPECT_EQ(run_command(Commands::help(), {"saver", "help"}), 0);
        fs::remove(file);
    }

    TEST(HelpTest, ReturnsOneWhenFileCannotBeOpened) {
        if (geteuid() == 0) GTEST_SKIP() << "root ignores file permissions";
        fs::path file = fs::temp_directory_path() / "saver_help_noperm.txt";
        std::ofstream(file) << "x\n";
        fs::permissions(file, fs::perms::none);
        MockResourcePath() = file;

        EXPECT_EQ(run_command(Commands::help(), {"saver", "help"}), 1);
        fs::remove(file);
    }
}

