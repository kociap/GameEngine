#include <core/paths.hpp>

#include <core/atl/string.hpp>
#include <core/paths_internal.hpp>
#include <core/utils/filesystem.hpp>
#include <core/filesystem.hpp>

namespace anton_engine::paths {
    static atl::String _executable_name;
    static atl::String _executable_directory;
    static atl::String _assets_directory;
    static atl::String _shaders_directory;

    void set_executable_name(atl::String_View const name) {
        _executable_name = name;
    }

    void set_executable_directory(atl::String_View const path) {
        _executable_directory = path;
        _assets_directory = fs::concat_paths(_executable_directory, "assets");
        _shaders_directory = fs::concat_paths(_executable_directory, "shaders");
    }

    atl::String_View executable_name() {
        return _executable_name;
    }

    atl::String_View executable_directory() {
        return _executable_directory;
    }

    atl::String_View assets_directory() {
        return _assets_directory;
    }

    atl::String_View shaders_directory() {
        // TODO a different path
        return _shaders_directory;
    }

#if ANTON_WITH_EDITOR
    static atl::String _project_directory;

    void set_project_directory(atl::String_View const path) {
        _project_directory = path;
    }

    atl::String_View project_directory() {
        return _project_directory;
    }
#endif // ANTON_WITH_EDITOR
} // namespace anton_engine::paths
