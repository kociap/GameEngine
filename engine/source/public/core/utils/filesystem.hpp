#ifndef CORE_UTILS_FILESYSTEM_HPP_INCLUDE
#define CORE_UTILS_FILESYSTEM_HPP_INCLUDE

#include <core/stl/vector.hpp>
#include <cstdint>
#include <filesystem>
#include <fstream>

namespace anton_engine::utils {
    // Path utility functions

    std::filesystem::path concat_paths(std::filesystem::path const&, std::filesystem::path const&);

    // File utility functions

    int64_t get_file_size(std::ifstream&);
    anton_stl::Vector<uint8_t> read_file_binary(std::filesystem::path const& path);
} // namespace anton_engine::utils

#endif // !CORE_UTILS_FILESYSTEM_HPP_INCLUDE