#include <core/filesystem.hpp>

#include <filesystem>

namespace anton_engine::fs {
    static atl::String fs_path_to_string(std::filesystem::path const& path) {
        std::string gen_str = path.generic_string();
        return {gen_str.data(), (i64)gen_str.size()};
    }

    atl::String concat_paths(atl::String_View const lhs, atl::String_View const rhs) {
        std::filesystem::path a(lhs.data());
        std::filesystem::path b(rhs.data());
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
} // namespace anton_engine::fs
