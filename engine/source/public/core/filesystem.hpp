#ifndef CORE_FILESYSTEM_HPP_INCLUDE
#define CORE_FILESYSTEM_HPP_INCLUDE

#include <core/atl/string_view.hpp>
#include <core/atl/string.hpp>

namespace anton_engine {
    atl::String concat_paths(atl::String_View, atl::String_View);
    atl::String concat_paths_with_separator(atl::String_View, atl::String_View);
    atl::String_View remove_filename(atl::String_View);
    atl::String_View remove_extension(atl::String_View);
    atl::String_View get_extension(atl::String_View);
    atl::String_View get_filename(atl::String_View);
    bool has_extension(atl::String_View);
    atl::String_View get_directory_name(atl::String_View);
}

#endif // !CORE_FILESYSTEM_HPP_INCLUDE
