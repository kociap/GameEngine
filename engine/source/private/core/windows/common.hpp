#ifndef CORE_WINDOWS_COMMON_HPP_INCLUDE
#define CORE_WINDOWS_COMMON_HPP_INCLUDE

#include <core/stl/string.hpp>
#include <cstdint>

namespace anton_engine::windows {
    [[nodiscard]] uint64_t get_last_error();
    [[nodiscard]] anton_stl::String get_last_error_message();
} // namespace anton_engine::windows

#endif // !CORE_WINDOWS_COMMON_HPP_INCLUDE
