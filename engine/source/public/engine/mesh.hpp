#ifndef ENGINE_MESH_HPP_INCLUDE
#define ENGINE_MESH_HPP_INCLUDE

#include <core/types.hpp>
#include <core/stl/type_traits.hpp>
#include <core/stl/vector.hpp>
#include <core/math/vector2.hpp>
#include <core/math/vector3.hpp>

namespace anton_engine {
    struct Vertex {
        Vector3 position;
        Vector3 normal;
        Vector3 tangent;
        Vector3 bitangent;
        Vector2 uv_coordinates;

        Vertex() = default;
        Vertex(Vector3 const pos, Vector3 const normal, Vector3 const tangent, Vector3 const bitangent, Vector2 const uv)
            : position(anton_stl::move(pos)), normal(anton_stl::move(normal)), tangent(anton_stl::move(tangent)), bitangent(anton_stl::move(bitangent)),
              uv_coordinates(anton_stl::move(uv)) {}
    };

    class Mesh {
    public:
        Mesh(anton_stl::Vector<Vertex> const& vertices, anton_stl::Vector<u32> const& indices): vertices(vertices), indices(indices) {}
        Mesh(anton_stl::Vector<Vertex>&& vertices, anton_stl::Vector<u32>&& indices): vertices(std::move(vertices)), indices(std::move(indices)) {}

        anton_stl::Vector<Vertex> vertices;
        anton_stl::Vector<u32> indices;
    };

    Mesh generate_plane();
    Mesh generate_cube();
} // namespace anton_engine

#endif // !ENGINE_MESH_HPP_INCLUDE
