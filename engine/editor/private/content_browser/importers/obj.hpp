#ifndef EDITOR_ASSET_IMPORTER_IMPORTERS_OBJ_HPP_INCLUDE
#define EDITOR_ASSET_IMPORTER_IMPORTERS_OBJ_HPP_INCLUDE

#include <content_browser/importers/mesh.hpp>
#include <core/atl/slice.hpp>
#include <core/atl/string_view.hpp>
#include <core/types.hpp>

namespace anton_engine::importers {
    [[nodiscard]] bool test_obj(atl::String_View file_extension, atl::Slice<u8 const> obj_data);
    [[nodiscard]] atl::Array<Mesh> import_obj(atl::Array<u8> const& obj_data);
} // namespace anton_engine::importers

#endif // !EDITOR_ASSET_IMPORTER_IMPORTERS_OBJ_HPP_INCLUDE
