#ifndef CORE_PATHS_INTERNAL_HPP_INCLUDE
#define CORE_PATHS_INTERNAL_HPP_INCLUDE

#include <build_config.hpp>

#include <filesystem>

namespace anton_engine::paths {
    void set_executable_name(std::filesystem::path);
    void set_executable_directory(std::filesystem::path);

#if ANTON_WITH_EDITOR
    void set_project_directory(std::filesystem::path);
#endif // ANTON_WITH_EDITOR
} // namespace anton_engine::paths

#endif // !CORE_PATHS_INTERNAL_HPP_INCLUDE
