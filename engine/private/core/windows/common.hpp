#pragma once

#include <anton/string.hpp>
#include <core/types.hpp>

namespace anton_engine::windows {
  [[nodiscard]] u64 get_last_error();
  [[nodiscard]] anton::String get_last_error_message();
} // namespace anton_engine::windows
