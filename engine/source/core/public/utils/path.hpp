#ifndef CORE_UTILS_PATH_HPP_INCLUDE
#define CORE_UTILS_PATH_HPP_INCLUDE

#include <filesystem>

namespace utils {
    std::filesystem::path concat_paths(std::filesystem::path const&, std::filesystem::path const&);
}

#endif // !CORE_UTILS_PATH_HPP_INCLUDE