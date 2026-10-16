#pragma once
#include <gmock/gmock.h>
#include <optional>
#include <filesystem>

namespace fs = std::filesystem;


class ProjectImplMockController {
public:
    MOCK_METHOD(std::optional<fs::path>, find_project_root, ());
};

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
inline ProjectImplMockController* g_project_impl_mock = nullptr;
