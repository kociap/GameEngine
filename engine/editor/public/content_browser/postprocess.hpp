#pragma once

#include <anton/math/math.hpp>
#include <anton/math/vec2.hpp>
#include <anton/math/vec3.hpp>
#include <anton/slice.hpp>
#include <engine/mesh.hpp>

namespace anton_engine {
  inline void
  flip_texture_coordinates(anton::Slice<Vec3> const texture_coordinates)
  {
    for(Vec3& coords: texture_coordinates) {
      coords.y = 1.0f - coords.y;
    }
  }

  inline void
  flip_texture_coordinates(anton::Slice<Vec2> const texture_coordinates)
  {
    for(Vec2& coords: texture_coordinates) {
      coords.y = 1.0f - coords.y;
    }
  }

  inline void flip_texture_coordinates(anton::Slice<Vertex> const vertices)
  {
    for(Vertex& vertex: vertices) {
      vertex.uv_coordinates.y = 1.0f - vertex.uv_coordinates.y;
    }
  }

} // namespace anton_engine
