#pragma once

#include <anton/array.hpp>
#include <anton/math/vec2.hpp>
#include <anton/math/vec3.hpp>
#include <anton/type_traits.hpp>
#include <core/types.hpp>

namespace anton_engine {
  struct Vertex {
    Vec3 position;
    Vec3 normal;
    Vec3 tangent;
    Vec3 bitangent;
    Vec2 uv_coordinates;

    Vertex() = default;
    Vertex(Vec3 const pos, Vec3 const normal, Vec3 const tangent,
           Vec3 const bitangent, Vec2 const uv)
      : position(ANTON_MOV(pos)), normal(ANTON_MOV(normal)),
        tangent(ANTON_MOV(tangent)), bitangent(ANTON_MOV(bitangent)),
        uv_coordinates(ANTON_MOV(uv))
    {
    }
  };

  class Mesh {
  public:
    Mesh(anton::Array<Vertex> const& vertices, anton::Array<u32> const& indices)
      : vertices(vertices), indices(indices)
    {
    }
    Mesh(anton::Array<Vertex>&& vertices, anton::Array<u32>&& indices)
      : vertices(ANTON_MOV(vertices)), indices(ANTON_MOV(indices))
    {
    }

    anton::Array<Vertex> vertices;
    anton::Array<u32> indices;
  };

  Mesh generate_plane();
  Mesh generate_cube();
} // namespace anton_engine
