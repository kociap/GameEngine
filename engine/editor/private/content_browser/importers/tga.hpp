#ifndef EDITOR_ASSET_IMPORTER_IMPORTERS_TGA_HPP_INCLUDE
#define EDITOR_ASSET_IMPORTER_IMPORTERS_TGA_HPP_INCLUDE

#include <content_browser/importers/image.hpp>

namespace anton_engine::importers {
    // TGA2 test
    bool test_tga(atl::Array<u8> const&);
    // Supports only 8, 16, 24 and 32 bit images
    Image import_tga(atl::Array<u8> const&);
} // namespace anton_engine::importers

#endif // !EDITOR_ASSET_IMPORTER_IMPORTERS_TGA_HPP_INCLUDE
