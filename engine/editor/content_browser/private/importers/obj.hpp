#ifndef EDITOR_ASSET_IMPORTER_IMPORTERS_OBJ_HPP_INCLUDE
#define EDITOR_ASSET_IMPORTER_IMPORTERS_OBJ_HPP_INCLUDE

#include <anton_int.hpp>
#include <anton_stl/slice.hpp>
#include <anton_stl/string_view.hpp>
#include <importers/mesh.hpp>

namespace anton_engine::importers {
    [[nodiscard]] bool test_obj(anton_stl::String_View file_extension, anton_stl::Slice<u8 const> obj_data);
    [[nodiscard]] anton_stl::Vector<Mesh> import_obj(anton_stl::Vector<uint8_t> const& obj_data);
} // namespace anton_engine::importers

#endif // !EDITOR_ASSET_IMPORTER_IMPORTERS_OBJ_HPP_INCLUDE
