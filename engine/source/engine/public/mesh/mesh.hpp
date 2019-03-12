#ifndef ENGINE_MESH_HPP_INCLUDE
#define ENGINE_MESH_HPP_INCLUDE

#include "math/vector2.hpp"
#include "math/vector3.hpp"
#include "object.hpp"
#include <string>
#include <vector>

enum class Texture_Type {
    specular,
    diffuse,
    normal,
};

struct Vertex {
    Vector3 position;
    Vector3 normal;
    Vector3 tangent;
    Vector3 bitangent;
    Vector2 uv_coordinates;

    Vertex() = default;
    Vertex(Vector3 pos, Vector3 normal, Vector3 tangent, Vector3 bitangent, Vector2 uv);
};

struct Texture {
    std::string name;
    Texture_Type type;
    uint32_t id;
};

class Shader;
namespace renderer {
    class Renderer;
}

class Mesh : public Object {
public:
    Mesh();
    Mesh(std::vector<Vertex> const& vertices, std::vector<uint32_t> const& indices, std::vector<Texture> const& textures);
    Mesh(std::vector<Vertex>&& vertices, std::vector<uint32_t>&& indices, std::vector<Texture>&& textures);
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
    std::vector<Texture> textures;

private:
    uint32_t vao = 0;
    uint32_t vbo = 0;
    uint32_t ebo = 0;
};

#endif // !ENGINE_MESH_HPP_INCLUDE