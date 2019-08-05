#ifndef EDITOR_ASSET_IMPORTER_IMPORTERS_OBJ_HPP_INCLUDE
#define EDITOR_ASSET_IMPORTER_IMPORTERS_OBJ_HPP_INCLUDE

#include <importers/mesh.hpp>

namespace importers {
    bool test_obj(containers::Vector<uint8_t> const&);
    containers::Vector<Mesh> import_obj(containers::Vector<uint8_t> const& obj_data);
} // namespace importers

#endif // !EDITOR_ASSET_IMPORTER_IMPORTERS_OBJ_HPP_INCLUDE
