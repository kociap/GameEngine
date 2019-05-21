#ifndef ENGINE_MESH_HPP_INCLUDE
#define ENGINE_MESH_HPP_INCLUDE

#include "containers/vector.hpp"
#include "material.hpp"
#include "math/vector2.hpp"
#include "math/vector3.hpp"
#include <string>

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
    Mesh(containers::Vector<Vertex> const& vertices, containers::Vector<uint32_t> const& indices);
    Mesh(containers::Vector<Vertex>&& vertices, containers::Vector<uint32_t>&& indices);
    Mesh(Mesh&&) noexcept;
    Mesh& operator=(Mesh&&) noexcept;
    ~Mesh();

    Mesh(Mesh const&) = delete;
    Mesh& operator=(Mesh const&) = delete;

    uint32_t get_vao() const;

private:
    void prepare_mesh();

public:
    containers::Vector<Vertex> vertices;
    containers::Vector<uint32_t> indices;

private:
    uint32_t vao = 0;
    uint32_t vbo = 0;
    uint32_t ebo = 0;
};

#endif // !ENGINE_MESH_HPP_INCLUDE