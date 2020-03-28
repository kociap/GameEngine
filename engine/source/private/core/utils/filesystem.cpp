#include <core/utils/filesystem.hpp>

#include <stdio.h>

namespace anton_engine::utils {
    std::filesystem::path concat_paths(std::filesystem::path const& a, std::filesystem::path const& b) {
        std::filesystem::path copy(a);
        copy /= b; // TODO use generic separator
        return copy;
    }

    atl::Vector<u8> read_file_binary(std::filesystem::path const& _path) {
        std::string path = _path.generic_string();
        FILE* file = fopen(path.data(), "rb");
        if (!file) {
            // TODO: More specific exception that will actually allow
            //       me to dispaly useful information to the user.
            throw Exception(u8"Failed to read the file"); 
        }

        fseek(file, 0, SEEK_END);
        i64 const file_size = ftell(file);
        fseek(file, 0, SEEK_SET);
        atl::Vector<u8> file_contents(file_size);
        fread(reinterpret_cast<char*>(file_contents.data()), file_size, 1, file);
        fclose(file);
        return file_contents;
    }
} // namespace anton_engine::utils
