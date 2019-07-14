#ifndef CORE_PATHS_HPP_INCLUDE
#define CORE_PATHS_HPP_INCLUDE

#include <filesystem>

namespace paths {
    std::filesystem::path engine_executable_directory();
    std::filesystem::path project_directory();
} // namespace paths

#endif // !CORE_PATHS_HPP_INCLUDE
