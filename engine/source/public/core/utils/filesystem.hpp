#ifndef CORE_UTILS_FILESYSTEM_HPP_INCLUDE
#define CORE_UTILS_FILESYSTEM_HPP_INCLUDE

#include <core/atl/array.hpp>
#include <core/atl/string_view.hpp>
#include <core/types.hpp>

namespace anton_engine::utils {
    // File utility functions

    atl::Array<u8> read_file_binary(atl::String_View path);
} // namespace anton_engine::utils

#endif // !CORE_UTILS_FILESYSTEM_HPP_INCLUDE
