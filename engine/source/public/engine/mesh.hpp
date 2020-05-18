#ifndef ENGINE_MESH_HPP_INCLUDE
#define ENGINE_MESH_HPP_INCLUDE

#include <core/atl/array.hpp>
#include <core/atl/type_traits.hpp>
#include <core/math/vector2.hpp>
#include <core/math/vector3.hpp>
#include <core/types.hpp>

namespace anton_engine {
    struct Vertex {
        Vector3 position;
        Vector3 normal;
        Vector3 tangent;
        Vector3 bitangent;
        Vector2 uv_coordinates;

        Vertex() = default;
        Vertex(Vector3 const pos, Vector3 const normal, Vector3 const tangent, Vector3 const bitangent, Vector2 const uv)
            : position(atl::move(pos)), normal(atl::move(normal)), tangent(atl::move(tangent)), bitangent(atl::move(bitangent)), uv_coordinates(atl::move(uv)) {
        }
    };

    class Mesh {
    public:
        Mesh(atl::Array<Vertex> const& vertices, atl::Array<u32> const& indices): vertices(vertices), indices(indices) {}
        Mesh(atl::Array<Vertex>&& vertices, atl::Array<u32>&& indices): vertices(atl::move(vertices)), indices(atl::move(indices)) {}

        atl::Array<Vertex> vertices;
        atl::Array<u32> indices;
    };

    Mesh generate_plane();
    Mesh generate_cube();
} // namespace anton_engine

#endif // !ENGINE_MESH_HPP_INCLUDE
