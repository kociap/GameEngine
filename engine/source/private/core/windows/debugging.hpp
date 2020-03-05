#ifndef CORE_WINDOW_DEBUGGING_HPP_INCLUDE
#define CORE_WINDOW_DEBUGGING_HPP_INCLUDE

#include <core/stl/string.hpp>
#include <cstdint>

namespace anton_engine::windows::debugging {

    [[nodiscard]] anton_stl::String get_type_as_string(void* process_handle, uint64_t module_base, uint64_t index);
} // namespace anton_engine::windows::debugging

#endif // !CORE_WINDOW_DEBUGGING_HPP_INCLUDE
