#ifndef CORE_UTILS_FILESYSTEM_HPP_INCLUDE
#define CORE_UTILS_FILESYSTEM_HPP_INCLUDE

#include <core/types.hpp>
#include <core/atl/vector.hpp>
#include <core/atl/string_view.hpp>

namespace anton_engine::utils {
    // File utility functions

    atl::Vector<u8> read_file_binary(atl::String_View path);
} // namespace anton_engine::utils

#endif // !CORE_UTILS_FILESYSTEM_HPP_INCLUDE
