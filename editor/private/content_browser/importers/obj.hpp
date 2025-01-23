#pragma once

#include <anton/slice.hpp>
#include <anton/string_view.hpp>
#include <content_browser/importers/mesh.hpp>
#include <core/types.hpp>

namespace anton_engine::importers {
  [[nodiscard]] bool test_obj(anton::String_View file_extension,
                              anton::Slice<u8 const> obj_data);
  [[nodiscard]] anton::Array<Mesh> import_obj(anton::Array<u8> const& obj_data);
} // namespace anton_engine::importers
