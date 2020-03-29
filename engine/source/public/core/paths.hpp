#ifndef CORE_PATHS_HPP_INCLUDE
#define CORE_PATHS_HPP_INCLUDE

#include <build_config.hpp>
#include <core/atl/string_view.hpp>

namespace anton_engine::paths {
    atl::String_View executable_name();
    atl::String_View executable_directory();

    atl::String_View assets_directory();
    atl::String_View shaders_directory();

#if ANTON_WITH_EDITOR
    atl::String_View project_directory();
#endif // ANTON_WITH_EDITOR
} // namespace anton_engine::paths

#endif // !CORE_PATHS_HPP_INCLUDE
