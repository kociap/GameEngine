#pragma once

#include <anton/array.hpp>
#include <anton/string_view.hpp>
#include <core/types.hpp>

namespace anton_engine::utils {
  // File utility functions

  anton::Array<u8> read_file_binary(anton::String_View path);
} // namespace anton_engine::utils
