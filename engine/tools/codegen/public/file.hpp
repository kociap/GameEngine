#ifndef TOOLS_CODEGEN_FILE_HPP_INCLUDE
#define TOOLS_CODEGEN_FILE_HPP_INCLUDE

#include <core/atl/array.hpp>
#include <filesystem>
#include <string_view>

namespace anton_engine {
    class File {
    public:
        using iterator = atl::Array<char>::iterator;

        File();
        File(std::string_view filename);
        File(std::filesystem::path filename);

        void read_file(std::string_view filename);
        void read_file(std::filesystem::path filename);
        void write_file(std::string_view filename);

        iterator begin();
        iterator end();

    private:
        atl::Array<char> contents;
    };
} // namespace anton_engine

#endif // !TOOLS_CODEGEN_FILE_HPP_INCLUDE
