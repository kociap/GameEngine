#include <core/filesystem.hpp>

#include <core/anton_crt.hpp>
#include <core/exception.hpp>

#include <filesystem>
#include <string_view>

namespace anton_engine::fs {
    static atl::String fs_path_to_string(std::filesystem::path const& path) {
        std::string gen_str = path.generic_string();
        return {gen_str.data(), (i64)gen_str.size()};
    }

    atl::String normalize_path(atl::String_View const path) {
        std::filesystem::path p(std::string_view(path.data(), path.size_bytes()));
        return fs_path_to_string(p);
    }

    atl::String concat_paths(atl::String_View const lhs, atl::String_View const rhs) {
        std::filesystem::path a(std::string_view(lhs.data(), lhs.size_bytes()));
        std::filesystem::path b(std::string_view(rhs.data(), rhs.size_bytes()));
        a /= b;
        return fs_path_to_string(a);
    }

    atl::String_View remove_filename(atl::String_View const path) {
        for(auto i = path.chars_end() - 1, begin = path.chars_begin() - 1; i != begin; --i) {
            char32 const c = *i;
            if(c == U'/' || c == U'\\') {
                return {path.chars_begin(), i + 1};
            }
        }
        return path;
    }

    atl::String_View remove_extension(atl::String_View const path) {
        auto i = path.chars_end() - 1;
        for(auto const begin = path.chars_begin() - 1; i != begin; --i) {
            char32 const c = *i;
            if(c == U'/' || c == U'\\') {
                return path;
            } else if(c == U'.') {
                return {path.chars_begin(), i};
            }
        }
        return path;
    }

    atl::String_View get_filename(atl::String_View const path) {
        for(auto i = path.chars_end() - 1, begin = path.chars_begin() - 1; i != begin; --i) {
            char32 const c = *i;
            if(c == U'/' || c == U'\\') {
                return {i + 1, path.chars_end()};
            }
        }
        return {};
    }

    atl::String_View get_filename_no_extension(atl::String_View const path) {
        // TODO: Improve.
        atl::String_View const filename = get_filename(path);
        return remove_extension(filename);
    }

    atl::String_View get_extension(atl::String_View const path) {
        auto i = path.chars_end() - 1;
        for(auto const begin = path.chars_begin() - 1; i != begin; --i) {
            char32 const c = *i;
            if(c == U'/' || c == U'\\') {
                return {};
            } else if(c == U'.') {
                return {i, path.chars_end()};
            }
        }
        return {};
    }

    atl::String_View get_directory_name(atl::String_View const path) {
        // TODO: Doesn't correctly handle drives or path root
        auto i = path.chars_end() - 1;
        for(auto const begin = path.chars_begin() - 1; i != begin; --i) {
            char32 const c = *i;
            if(c == U'/' || c == U'\\') {
                return {path.chars_begin(), i};
            }
        }
        return {};
    }

    bool has_filename(atl::String_View const path) {
        atl::String_View const filename = get_filename(path);
        return filename.size_bytes();
    }

    bool has_extension(atl::String_View const path) {
        atl::String_View const ext = get_extension(path);
        return ext.size_bytes();
    }

    bool exists(atl::String_View const path) {
        std::filesystem::path a(std::string_view(path.data(), path.size_bytes()));
        return std::filesystem::exists(a);
    }

    Input_File_Stream::Input_File_Stream(): _buffer(nullptr) {}

    Input_File_Stream::Input_File_Stream(atl::String const& filename): _buffer(nullptr) {
        open(filename);
    }

    Input_File_Stream::Input_File_Stream(Input_File_Stream&& other): _buffer(other._buffer) {
        other._buffer = nullptr;
    }

    Input_File_Stream& Input_File_Stream::operator=(Input_File_Stream&& other) {
        atl::swap(_buffer, other._buffer);
        return *this;
    }

    Input_File_Stream::~Input_File_Stream() {
        close();
    }

    Input_File_Stream::operator bool() const {
        return is_open();
    }

    bool Input_File_Stream::open(atl::String const& filename) {
        if(_buffer) {
            fclose((FILE*)_buffer);
        }

        _buffer = fopen(filename.data(), "rb");
        return _buffer != nullptr;
    }

    void Input_File_Stream::close() {
        if(_buffer) {
            fclose((FILE*)_buffer);
        }
    }

    bool Input_File_Stream::is_open() const {
        return _buffer != nullptr;
    }

    void Input_File_Stream::read(void* buffer, i64 count) {
        fread(buffer, count, 1, (FILE*)_buffer);
    }

    void Input_File_Stream::read(atl::Slice<u8> const buffer) {
        fread(buffer.data(), buffer.size(), 1, (FILE*)_buffer);
    }

    char32 Input_File_Stream::peek() {
        char32 const c = get();
        unget(c);
        return c;
    }

    char32 Input_File_Stream::get() {
        return fgetc((FILE*)_buffer);
    }

    void Input_File_Stream::unget(char32 c) {
        ungetc(c, (FILE*)_buffer);
    }

    void Input_File_Stream::seek(Seek_Dir dir, i64 offset) {
        fseek((FILE*)_buffer, offset, (int)(dir));
    }

    i64 Input_File_Stream::tell() {
        return ftell((FILE*)_buffer);
    }

    Output_File_Stream::Output_File_Stream(): _buffer(nullptr) {}

    Output_File_Stream::Output_File_Stream(atl::String const& filename): _buffer(nullptr) {
        open(filename);
    }

    Output_File_Stream::Output_File_Stream(Output_File_Stream&& other): _buffer(other._buffer) {
        other._buffer = nullptr;
    }

    Output_File_Stream& Output_File_Stream::operator=(Output_File_Stream&& other) {
        atl::swap(_buffer, other._buffer);
        return *this;
    }

    Output_File_Stream::~Output_File_Stream() {
        close();
    }

    Output_File_Stream::operator bool() const {
        return is_open();
    }

    bool Output_File_Stream::open(atl::String const& filename) {
        if(_buffer) {
            fclose((FILE*)_buffer);
        }

        _buffer = fopen(filename.data(), "wb");
        return _buffer != nullptr;
    }

    void Output_File_Stream::close() {
        if(_buffer) {
            fclose((FILE*)_buffer);
        }
    }

    bool Output_File_Stream::is_open() const {
        return _buffer != nullptr;
    }

    void Output_File_Stream::flush() {
        fflush((FILE*)_buffer);
    }

    void Output_File_Stream::write(void const* buffer, i64 count) {
        fwrite(buffer, count, 1, (FILE*)_buffer);
    }

    void Output_File_Stream::write(atl::Slice<u8 const> const buffer) {
        fwrite(buffer.data(), buffer.size(), 1, (FILE*)_buffer);
    }

    void Output_File_Stream::write(atl::String_View const buffer) {
        fwrite(buffer.data(), buffer.size_bytes(), 1, (FILE*)_buffer);
    }

    void Output_File_Stream::put(char32 c) {
        fputc(c, (FILE*)_buffer);
    }

    void Output_File_Stream::seek(Seek_Dir dir, i64 offset) {
        fseek((FILE*)_buffer, offset, (int)(dir));
    }

    i64 Output_File_Stream::tell() {
        return ftell((FILE*)_buffer);
    }
} // namespace anton_engine::fs
