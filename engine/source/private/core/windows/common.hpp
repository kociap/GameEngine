#ifndef CORE_WINDOWS_COMMON_HPP_INCLUDE
#define CORE_WINDOWS_COMMON_HPP_INCLUDE

#include <core/atl/string.hpp>
#include <core/types.hpp>

namespace anton_engine::windows {
    [[nodiscard]] u64 get_last_error();
    [[nodiscard]] atl::String get_last_error_message();
} // namespace anton_engine::windows

#endif // !CORE_WINDOWS_COMMON_HPP_INCLUDE
