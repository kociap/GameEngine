#ifndef TOOLS_CODEGEN_COMPONENT_HEADER_PARSER_HPP_INCLUDE
#define TOOLS_CODEGEN_COMPONENT_HEADER_PARSER_HPP_INCLUDE

#include <core/atl/array.hpp>
#include <file.hpp>
#include <string>

namespace anton_engine {
    atl::Array<std::string> parse_component_header(File& file);
}

#endif // !TOOLS_CODEGEN_COMPONENT_HEADER_PARSER_HPP_INCLUDE
