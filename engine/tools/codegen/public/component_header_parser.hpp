#ifndef TOOLS_CODEGEN_COMPONENT_HEADER_PARSER_HPP_INCLUDE
#define TOOLS_CODEGEN_COMPONENT_HEADER_PARSER_HPP_INCLUDE

#include <containers/vector.hpp>
#include <file.hpp>
#include <string>

containers::Vector<std::string> parse_component_header(File& file);

#endif // !TOOLS_CODEGEN_COMPONENT_HEADER_PARSER_HPP_INCLUDE
