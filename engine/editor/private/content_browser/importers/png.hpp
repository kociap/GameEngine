#ifndef ENGINE_ASSET_IMPORTER_IMPORTERS_PNG_HPP_INCLUDE
#define ENGINE_ASSET_IMPORTER_IMPORTERS_PNG_HPP_INCLUDE

#include <content_browser/importers/image.hpp>

namespace anton_engine::importers {
    bool test_png(atl::Array<u8> const&);
    Image import_png(atl::Array<u8> const&);
} // namespace anton_engine::importers

#endif // !ENGINE_ASSET_IMPORTER_IMPORTERS_PNG_HPP_INCLUDE
