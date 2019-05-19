#ifndef ENGINE_MESH_HPP_INCLUDE
#define ENGINE_MESH_HPP_INCLUDE

#include "material.hpp"
#include "math/vector2.hpp"
#include "math/vector3.hpp"
#include <string>
#include <vector>

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
    Mesh(std::vector<Vertex> const& vertices, std::vector<uint32_t> const& indices, Material);
    Mesh(std::vector<Vertex>&& vertices, std::vector<uint32_t>&& indices, Material);
    Mesh(Mesh&&) noexcept;
    Mesh& operator=(Mesh&&) noexcept;
    ~Mesh();

    Mesh(Mesh const&) = delete;
    Mesh& operator=(Mesh const&) = delete;

    uint32_t get_vao() const;

private:
    void prepare_mesh();

public:
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    Material material;

private:
    uint32_t vao = 0;
    uint32_t vbo = 0;
    uint32_t ebo = 0;
};

#endif // !ENGINE_MESH_HPP_INCLUDE