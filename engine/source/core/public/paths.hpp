#ifndef CORE_PATHS_HPP_INCLUDE
#define CORE_PATHS_HPP_INCLUDE

#include <build_config.hpp>
#include <filesystem>

namespace paths {
    std::filesystem::path engine_executable_name();
    std::filesystem::path engine_executable_directory();
    std::filesystem::path project_directory();

#if !GE_BUILD_SHIPPING
    // Non-shipping build only
    std::filesystem::path assets_directory();
    std::filesystem::path shaders_directory();
#endif
} // namespace paths

#endif // !CORE_PATHS_HPP_INCLUDE
