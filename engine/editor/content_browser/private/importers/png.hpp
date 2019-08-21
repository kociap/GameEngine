#ifndef ENGINE_ASSET_IMPORTER_IMPORTERS_PNG_HPP_INCLUDE
#define ENGINE_ASSET_IMPORTER_IMPORTERS_PNG_HPP_INCLUDE

#include <importers/image.hpp>

namespace importers {
    bool test_png(anton_stl::Vector<uint8_t> const&);
    Image import_png(anton_stl::Vector<uint8_t> const&);
} // namespace importers

#endif // !ENGINE_ASSET_IMPORTER_IMPORTERS_PNG_HPP_INCLUDE
