#pragma once

#include <anton/string_view.hpp>

namespace anton_engine::paths {
  anton::String_View executable_name();
  anton::String_View executable_directory();

  anton::String_View assets_directory();
  anton::String_View shaders_directory();

  // TODO: Move to editor paths
#if ANTON_WITH_EDITOR
  anton::String_View project_directory();
#endif // ANTON_WITH_EDITOR
} // namespace anton_engine::paths
