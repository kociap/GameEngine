#pragma once

#include <content_browser/importers/image.hpp>

namespace anton_engine::importers {
  bool test_png(anton::Array<u8> const&);
  Image import_png(anton::Array<u8> const&);
} // namespace anton_engine::importers
