#ifndef TOOLS_CODEGEN_COMPONENT_HEADER_PARSER_HPP_INCLUDE
#define TOOLS_CODEGEN_COMPONENT_HEADER_PARSER_HPP_INCLUDE

#include <core/stl/vector.hpp>
#include <file.hpp>
#include <string>

namespace anton_engine {
    anton_stl::Vector<std::string> parse_component_header(File& file);
}

#endif // !TOOLS_CODEGEN_COMPONENT_HEADER_PARSER_HPP_INCLUDE
