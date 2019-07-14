#ifndef CORE_UTILS_FILESYSTEM_HPP_INCLUDE
#define CORE_UTILS_FILESYSTEM_HPP_INCLUDE

#include <containers/vector.hpp>
#include <cstdint>
#include <filesystem>
#include <fstream>

namespace utils {
    // Path utility functions

    std::filesystem::path concat_paths(std::filesystem::path const&, std::filesystem::path const&);

    // File utility functions

    uint64_t get_file_size(std::ifstream&);
    containers::Vector<uint8_t> read_file_binary(std::filesystem::path const& path);
} // namespace utils

#endif // !CORE_UTILS_FILESYSTEM_HPP_INCLUDE
