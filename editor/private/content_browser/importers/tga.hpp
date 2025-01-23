#pragma once

#include <content_browser/importers/image.hpp>

namespace anton_engine::importers {
  // TGA2 test
  bool test_tga(anton::Array<u8> const&);
  // Supports only 8, 16, 24 and 32 bit images
  Image import_tga(anton::Array<u8> const&);
} // namespace anton_engine::importers
