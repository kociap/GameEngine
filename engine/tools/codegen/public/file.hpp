#ifndef TOOLS_CODEGEN_FILE_HPP_INCLUDE
#define TOOLS_CODEGEN_FILE_HPP_INCLUDE

#include <containers/vector.hpp>
#include <filesystem>
#include <string_view>

class File {
public:
    using iterator = containers::Vector<char>::iterator;

    File();
    File(std::string_view filename);
    File(std::filesystem::path filename);

    void read_file(std::string_view filename);
    void read_file(std::filesystem::path filename);
    void write_file(std::string_view filename);

    iterator begin();
    iterator end();

private:
    containers::Vector<char> contents;
};

#endif // !TOOLS_CODEGEN_FILE_HPP_INCLUDE
