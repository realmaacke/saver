#include <gtest/gtest.h>
#include "Commands.hpp"

TEST(CommandTest, ReturnSuccess) {
    auto help = Commands::help();
}