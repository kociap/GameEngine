#ifndef CORE_PATHS_INTERNAL_HPP_INCLUDE
#define CORE_PATHS_INTERNAL_HPP_INCLUDE

#include <build_config.hpp>
#include <core/atl/string_view.hpp>

namespace anton_engine::paths {
    void set_executable_name(atl::String_View path);
    void set_executable_directory(atl::String_View path);

#if ANTON_WITH_EDITOR
    void set_project_directory(atl::String_View path);
#endif // ANTON_WITH_EDITOR
} // namespace anton_engine::paths

#endif // !CORE_PATHS_INTERNAL_HPP_INCLUDE
