#ifndef EDITOR_ASSET_IMPORTER_IMPORTERS_OBJ_HPP_INCLUDE
#define EDITOR_ASSET_IMPORTER_IMPORTERS_OBJ_HPP_INCLUDE

#include <importers/mesh.hpp>

namespace importers {
    bool test_obj(anton_stl::Vector<uint8_t> const&);
    anton_stl::Vector<Mesh> import_obj(anton_stl::Vector<uint8_t> const& obj_data);
} // namespace importers

#endif // !EDITOR_ASSET_IMPORTER_IMPORTERS_OBJ_HPP_INCLUDE
