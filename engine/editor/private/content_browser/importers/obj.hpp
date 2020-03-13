#ifndef EDITOR_ASSET_IMPORTER_IMPORTERS_OBJ_HPP_INCLUDE
#define EDITOR_ASSET_IMPORTER_IMPORTERS_OBJ_HPP_INCLUDE

#include <core/types.hpp>
#include <core/atl/slice.hpp>
#include <core/atl/string_view.hpp>
#include <content_browser/importers/mesh.hpp>

namespace anton_engine::importers {
    [[nodiscard]] bool test_obj(atl::String_View file_extension, atl::Slice<u8 const> obj_data);
    [[nodiscard]] atl::Vector<Mesh> import_obj(atl::Vector<uint8_t> const& obj_data);
} // namespace anton_engine::importers

#endif // !EDITOR_ASSET_IMPORTER_IMPORTERS_OBJ_HPP_INCLUDE
