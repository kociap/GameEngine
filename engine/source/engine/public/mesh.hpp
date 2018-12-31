#ifndef ENGINE_MESH_HPP_INCLUDE
#define ENGINE_MESH_HPP_INCLUDE

#include "math/vector2.hpp"
#include "math/vector3.hpp"
#include "shader.hpp"
#include <vector>
#include <string>

enum class Texture_type { 
	specular, 
	diffuse, 
};

struct Vertex {
    Vector3 position;
    Vector3 normal;
    Vector2 uv_coordinates;

	Vertex() = default;
	Vertex(Vector3, Vector3, Vector2);
};

struct Texture {
    std::string name;
    Texture_type type;
    uint32_t id;
};

class Mesh {
public:
    Mesh();
    Mesh(std::vector<Vertex> const& vertices, std::vector<uint32_t> const& indices, std::vector<Texture> const& textures);
    Mesh(std::vector<Vertex>&& vertices, std::vector<uint32_t>&& indices, std::vector<Texture>&& textures);
    Mesh(Mesh&&);
    Mesh& operator=(Mesh&&);
    ~Mesh();

    Mesh(Mesh const&) = delete;
    Mesh& operator=(Mesh const&) = delete;

	void draw(Shader&);

private:
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    std::vector<Texture> textures;
    uint32_t vao = 0;
    uint32_t vbo = 0;
    uint32_t ebo = 0;

    void prepare_mesh();
};

#endif // !ENGINE_MESH_HPP_INCLUDE