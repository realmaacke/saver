#include "gmock/gmock.h"
#include <fstream>
#include <gtest/gtest.h>
#include <filesystem>
#include <initializer_list>
#include <optional>
#include <string>
#include <vector>
#include <unistd.h>

#include "Output/Codes.hpp"
#include "Commands.hpp"
#include "Environment.hpp"
#include "Output/Codes/InformationCodes.hpp"

#include "ProjectImplMock.hpp"
#include "stdout.hpp"
#include "tests/mocks/Environment.hpp"


namespace fs = std::filesystem;

namespace {
    inline int run_command(const cmd& command, std::initializer_list<const char*> args) {
        std::vector<std::string> arg_buffers(args.begin(), args.end());
        std::vector<char*> argv;
        argv.reserve(arg_buffers.size() + 1);
    
        for (std::basic_string<char>& arg : arg_buffers) {
            argv.push_back(arg.data());
        }
        argv.push_back(nullptr);
        return command(static_cast<int>(arg_buffers.size()), argv.data());
    }
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
        if (geteuid() == 0) { GTEST_SKIP() << "root ignores file permissions"; }
        const fs::path file = fs::temp_directory_path() / "saver_help_noperm.txt";
        std::ofstream(file) << "x\n";
        fs::permissions(file, fs::perms::none);
        MockResourcePath() = file;

        EXPECT_EQ(run_command(Commands::help(), {"saver", "help"}), 1);
        fs::remove(file);
    }
}

namespace commands_add {
    TEST(AddTest, ReturnsZeroWithLessArgs) {
        const std::string actual = 
            std::string(code_to_message(InfoCode::NO_PATH_SPECIFIED)) + "\n";
        const StdoutCapture out;
        EXPECT_EQ(run_command(Commands::add(), {"saver"}), 0);
        EXPECT_EQ(actual, out.str());
    }

    TEST(AddTest, ReturnsZeroNotInProject) {
        ProjectImplMockController mock_impl;
        g_project_impl_mock = &mock_impl;

        EXPECT_CALL(mock_impl, find_project_root())
            .WillOnce(testing::Return(std::nullopt));

        const StdoutCapture out;
        const std::string actual = 
            std::string(code_to_message(InfoCode::PROJECT_NOT_FOUND)) + "\n";
        EXPECT_EQ(run_command(Commands::add(), {"saver", "add", "."}), 0);
        EXPECT_EQ(actual, out.str());
    }
}

namespace commands_remove {
    TEST(RemoveTest, ReturnsZeroWithLessArgs) {
        const std::string actual = 
            std::string(code_to_message(InfoCode::NO_PATH_SPECIFIED)) + "\n";
        const StdoutCapture out;
        EXPECT_EQ(run_command(Commands::remove(), {"saver",}), 0);
        EXPECT_EQ(actual, out.str());
    }
}

// NOLINTNEXTLINE
