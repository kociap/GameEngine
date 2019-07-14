#ifndef CORE_PATHS_INTERNAL_HPP_INCLUDE
#define CORE_PATHS_INTERNAL_HPP_INCLUDE

#include <filesystem>

namespace paths {
    void set_engine_executable_directory(std::filesystem::path);
    void set_project_directory(std::filesystem::path);
} // namespace paths

#endif // !CORE_PATHS_INTERNAL_HPP_INCLUDE
