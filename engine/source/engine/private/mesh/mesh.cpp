#include "mesh/mesh.hpp"
#include "debug_macros.hpp"
#include "glad/glad.h"
#include "opengl.hpp"
#include "renderer.hpp"
#include "shader.hpp"

Vertex::Vertex(Vector3 pos, Vector3 norm, Vector3 tan, Vector3 bitan, Vector2 tex)
    : position(std::move(pos)), normal(std::move(norm)), tangent(std::move(tan)), bitangent(std::move(bitan)), uv_coordinates(std::move(tex)) {}

Mesh::Mesh() {}

Mesh::Mesh(std::vector<Vertex> const& vertices, std::vector<uint32_t> const& indices, std::vector<Texture> const& textures)
    : vertices(vertices), indices(indices), textures(textures) {
    prepare_mesh();
}

Mesh::Mesh(std::vector<Vertex>&& vertices, std::vector<uint32_t>&& indices, std::vector<Texture>&& textures)
    : vertices(std::move(vertices)), indices(std::move(indices)), textures(std::move(textures)) {
    prepare_mesh();
}

Mesh::Mesh(Mesh&& from) noexcept: vertices(std::move(from.vertices)), indices(std::move(from.indices)), textures(std::move(from.textures)) {
    std::swap(ebo, from.ebo);
    std::swap(vbo, from.vbo);
    std::swap(vao, from.vao);
}

Mesh& Mesh::operator=(Mesh&& from) noexcept {
    vertices = std::move(from.vertices);
    indices = std::move(from.indices);
    textures = std::move(from.textures);
    std::swap(ebo, from.ebo);
    std::swap(vbo, from.vbo);
    std::swap(vao, from.vao);
    return *this;
}

Mesh::~Mesh() {
    if (vao) {
        glDeleteVertexArrays(1, &vao);
    }
    if (vbo) {
        glDeleteBuffers(1, &vbo);
    }
    if (ebo) {
        glDeleteBuffers(1, &ebo);
    }
}

uint32_t Mesh::get_vao() const {
    return vao;
}

void Mesh::prepare_mesh() {
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    opengl::bind_vertex_array(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), &indices[0], GL_STATIC_DRAW);

    // position
    opengl::vertex_array_attribute(0, 3, GL_FLOAT, sizeof(Vertex), 0);
    opengl::enable_vertex_array_attribute(0);
    // normal
    opengl::vertex_array_attribute(1, 3, GL_FLOAT, sizeof(Vertex), sizeof(Vector3));
    opengl::enable_vertex_array_attribute(1);
    // tangent
    opengl::vertex_array_attribute(2, 3, GL_FLOAT, sizeof(Vertex), 2 * sizeof(Vector3));
    opengl::enable_vertex_array_attribute(2);
    // bitangent
    opengl::vertex_array_attribute(3, 3, GL_FLOAT, sizeof(Vertex), 3 * sizeof(Vector3));
    opengl::enable_vertex_array_attribute(3);
    // texture coordinates
    opengl::vertex_array_attribute(4, 2, GL_FLOAT, sizeof(Vertex), 4 * sizeof(Vector3));
    opengl::enable_vertex_array_attribute(4);

    opengl::bind_vertex_array(0);
}