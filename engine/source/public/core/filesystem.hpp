#ifndef CORE_FILESYSTEM_HPP_INCLUDE
#define CORE_FILESYSTEM_HPP_INCLUDE

#include <core/stl/string_view.hpp>
#include <core/stl/string.hpp>

namespace anton_engine {
    anton_stl::String concat_paths(anton_stl::String_View, anton_stl::String_View);
    anton_stl::String concat_paths_with_separator(anton_stl::String_View, anton_stl::String_View);
    anton_stl::String_View remove_filename(anton_stl::String_View);
    anton_stl::String_View remove_extension(anton_stl::String_View);
    anton_stl::String_View get_extension(anton_stl::String_View);
    anton_stl::String_View get_filename(anton_stl::String_View);
    bool has_extension(anton_stl::String_View);
    anton_stl::String_View get_directory_name(anton_stl::String_View);
}

#endif // !CORE_FILESYSTEM_HPP_INCLUDE
