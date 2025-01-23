#pragma once

#include <anton/array.hpp>
#include <anton/slice.hpp>
#include <anton/string_view.hpp>
#include <core/types.hpp>
#include <engine/mesh.hpp>

namespace anton_engine::asset_importing {
  class Imported_Meshes {
  public:
    anton::Array<i32> hierarchy;
    anton::Array<Mesh> meshes;
  };

  void import_image(anton::String_View path);
  Imported_Meshes import_mesh(anton::String_View path);

  void save_meshes(anton::String_View filename, anton::Slice<u64 const> guids,
                   anton::Slice<Mesh const> meshes);
}; // namespace anton_engine::asset_importing
