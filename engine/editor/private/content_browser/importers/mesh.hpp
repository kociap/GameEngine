#pragma once

#include <anton/array.hpp>
#include <anton/math/vec2.hpp>
#include <anton/math/vec3.hpp>
#include <anton/string.hpp>
#include <core/types.hpp>

namespace anton_engine::importers {
  class Face {
  public:
    anton::Array<u32> indices;
  };

  class Mesh {
  public:
    anton::String name;
    anton::Array<Face> faces;
    anton::Array<Vec3> vertices;
    anton::Array<Vec3> normals;
    anton::Array<Vec3> texture_coordinates;
  };
} // namespace anton_engine::importers
