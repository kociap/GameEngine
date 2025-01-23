#pragma once

#include <anton/string_view.hpp>
#include <build_config.hpp>

namespace anton_engine::paths {
  void set_executable_name(anton::String_View path);
  void set_executable_directory(anton::String_View path);

#if ANTON_WITH_EDITOR
  void set_project_directory(anton::String_View path);
#endif // ANTON_WITH_EDITOR
} // namespace anton_engine::paths
