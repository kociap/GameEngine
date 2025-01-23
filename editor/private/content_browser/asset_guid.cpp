#include <content_browser/asset_guid.hpp>

namespace anton_engine::asset_importing {
  u64 generate_asset_guid()
  {
    // TODO: Save last used guid in project files.
    static u64 guid = 0;
    return ++guid;
  }
} // namespace anton_engine::asset_importing
