#pragma once

#include <anton/array.hpp>
#include <file.hpp>
#include <string>

namespace anton_engine {
  anton::Array<std::string> parse_component_header(File& file);
}
