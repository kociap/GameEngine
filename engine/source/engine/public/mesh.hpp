#ifndef ENGINE_MESH_HPP_INCLUDE
#define ENGINE_MESH_HPP_INCLUDE

#include <anton_stl/vector.hpp>
#include <material.hpp>
#include <math/vector2.hpp>
#include <math/vector3.hpp>
#include <string>

namespace anton_engine {
    struct Vertex {
        Vector3 position;
        Vector3 normal;
        Vector3 tangent;
        Vector3 bitangent;
        Vector2 uv_coordinates;

        Vertex() = default;
        Vertex(Vector3 pos, Vector3 normal, Vector3 tangent, Vector3 bitangent, Vector2 uv);
    };

    class Mesh {
    public:
        Mesh();
        Mesh(anton_stl::Vector<Vertex> const& vertices, anton_stl::Vector<uint32_t> const& indices);
        Mesh(anton_stl::Vector<Vertex>&& vertices, anton_stl::Vector<uint32_t>&& indices);
        Mesh(Mesh&&) noexcept;
        Mesh& operator=(Mesh&&) noexcept;
        ~Mesh();

        Mesh(Mesh const&) = delete;
        Mesh& operator=(Mesh const&) = delete;

        uint32_t get_vbo() const;
        uint32_t get_ebo() const;

    private:
        void prepare_mesh();

    public:
        anton_stl::Vector<Vertex> vertices;
        anton_stl::Vector<uint32_t> indices;

    private:
        uint32_t vbo = 0;
        uint32_t ebo = 0;
    };

    Mesh generate_plane();
    Mesh generate_cube();
} // namespace anton_engine

#endif // !ENGINE_MESH_HPP_INCLUDE
