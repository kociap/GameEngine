#include <core/utils/filesystem.hpp>

#include <core/anton_crt.hpp>

namespace anton_engine::utils {
    atl::Array<u8> read_file_binary(atl::String_View const path) {
        FILE* file = fopen(path.data(), "rb");
        if(!file) {
            // TODO: More specific exception that will actually allow
            //       me to dispaly useful information to the user.
            throw Exception(u8"Failed to read the file");
        }

        fseek(file, 0, SEEK_END);
        i64 const file_size = ftell(file);
        fseek(file, 0, SEEK_SET);
        atl::Array<u8> file_contents(file_size);
        fread(reinterpret_cast<char*>(file_contents.data()), file_size, 1, file);
        fclose(file);
        return file_contents;
    }
} // namespace anton_engine::utils
