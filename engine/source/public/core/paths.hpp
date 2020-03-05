#ifndef CORE_PATHS_HPP_INCLUDE
#define CORE_PATHS_HPP_INCLUDE

#include <build_config.hpp>
#include <filesystem>

namespace anton_engine::paths {
    std::filesystem::path executable_name();
    std::filesystem::path executable_directory();

    std::filesystem::path assets_directory();
    std::filesystem::path shaders_directory();

#if ANTON_WITH_EDITOR
    std::filesystem::path project_directory();
#endif // ANTON_WITH_EDITOR
} // namespace anton_engine::paths

#endif // !CORE_PATHS_HPP_INCLUDE
