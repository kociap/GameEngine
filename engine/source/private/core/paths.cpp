#include <core/paths.hpp>

#include <anton/filesystem.hpp>
#include <anton/string.hpp>
#include <core/paths_internal.hpp>
#include <core/utils/filesystem.hpp>

namespace anton_engine::paths {
  static anton::String _executable_name;
  static anton::String _executable_directory;
  static anton::String _assets_directory;
  static anton::String _shaders_directory;

  void set_executable_name(anton::String_View const name)
  {
    _executable_name = name;
  }

  void set_executable_directory(anton::String_View const path)
  {
    _executable_directory = path;
    _assets_directory =
      anton::fs::concat_paths(_executable_directory, "assets");
    _shaders_directory =
      anton::fs::concat_paths(_executable_directory, "shaders");
  }

  anton::String_View executable_name()
  {
    return _executable_name;
  }

  anton::String_View executable_directory()
  {
    return _executable_directory;
  }

  anton::String_View assets_directory()
  {
    return _assets_directory;
  }

  anton::String_View shaders_directory()
  {
    // TODO a different path
    return _shaders_directory;
  }

#if ANTON_WITH_EDITOR
  static anton::String _project_directory;

  void set_project_directory(anton::String_View const path)
  {
    _project_directory = path;
  }

  anton::String_View project_directory()
  {
    return _project_directory;
  }
#endif // ANTON_WITH_EDITOR
} // namespace anton_engine::paths
