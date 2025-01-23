#pragma once

#include <anton/string.hpp>
#include <core/types.hpp>

namespace anton_engine::windows::debugging {
  [[nodiscard]] anton::String get_type_as_string(void* process_handle,
                                                 u64 module_base, u64 index);
} // namespace anton_engine::windows::debugging
