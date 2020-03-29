#ifndef CORE_FILESYSTEM_HPP_INCLUDE
#define CORE_FILESYSTEM_HPP_INCLUDE

#include <core/atl/string_view.hpp>
#include <core/atl/string.hpp>

namespace anton_engine::fs {
    atl::String concat_paths(atl::String_View path1, atl::String_View path2);
    atl::String_View remove_filename(atl::String_View path);
    atl::String_View remove_extension(atl::String_View path);
    atl::String_View get_filename(atl::String_View path);
    atl::String_View get_filename_no_extension(atl::String_View path);
    atl::String_View get_extension(atl::String_View path);
    atl::String_View get_directory_name(atl::String_View path);
    bool has_extension(atl::String_View path);
    bool has_filename(atl::String_View path);
} // namespace anton_engine::fs

#endif // !CORE_FILESYSTEM_HPP_INCLUDE
