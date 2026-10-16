#include "Project/ProjectImpl.hpp"
#include "ProjectImplMock.hpp"
#include <filesystem>
#include <optional>

std::optional<fs::path> ProjectImpl::find_project_root() {
    if (g_project_impl_mock) {
        return g_project_impl_mock->find_project_root();
    }
    return std::nullopt;
}

ProjectImpl::ProjectImpl() = default;