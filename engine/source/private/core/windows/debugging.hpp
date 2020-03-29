#ifndef CORE_WINDOW_DEBUGGING_HPP_INCLUDE
#define CORE_WINDOW_DEBUGGING_HPP_INCLUDE

#include <core/atl/string.hpp>
#include <core/types.hpp>

namespace anton_engine::windows::debugging {
    [[nodiscard]] atl::String get_type_as_string(void* process_handle, u64 module_base, u64 index);
} // namespace anton_engine::windows::debugging

#endif // !CORE_WINDOW_DEBUGGING_HPP_INCLUDE
