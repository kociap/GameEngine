#include "mesh/mesh.hpp"
#include "debug_macros.hpp"
#include "glad/glad.h"
#include "shader.hpp"

Vertex::Vertex(Vector3 pos, Vector3 norm, Vector2 tex) : position(std::move(pos)), normal(std::move(norm)), uv_coordinates(std::move(tex)) {}

Mesh::Mesh() {}

Mesh::Mesh(std::vector<Vertex> const& vertices, std::vector<uint32_t> const& indices, std::vector<Texture> const& textures)
    : vertices(vertices), indices(indices), textures(textures) {
    prepare_mesh();
}

Mesh::Mesh(std::vector<Vertex>&& vertices, std::vector<uint32_t>&& indices, std::vector<Texture>&& textures)
    : vertices(std::move(vertices)), indices(std::move(indices)), textures(std::move(textures)) {
    prepare_mesh();
}

Mesh::Mesh(Mesh&& from) noexcept
    : Object(std::move(from)), vertices(std::move(from.vertices)), indices(std::move(from.indices)), textures(std::move(from.textures)) {
    std::swap(ebo, from.ebo);
    std::swap(vbo, from.vbo);
    std::swap(vao, from.vao);
}

Mesh& Mesh::operator=(Mesh&& from) noexcept {
    Object::operator=(std::move(from));
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

void Mesh::draw(Shader& shader) {
    CHECK_GL_ERRORS();
    std::size_t specular = 0;
    std::size_t diffuse = 0;
    for (std::size_t i = 0; i < textures.size(); ++i) {
        glActiveTexture(GL_TEXTURE0 + i);
        CHECK_GL_ERRORS();
        if (textures[i].type == Texture_Type::diffuse) {
            shader.set_int("material.texture_diffuse" + std::to_string(diffuse), i);
            ++diffuse;
        } else {
            shader.set_int("material.texture_specular" + std::to_string(specular), i);
            ++specular;
        }
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
        CHECK_GL_ERRORS();
    }
    CHECK_GL_ERRORS();
    glBindVertexArray(vao);
    CHECK_GL_ERRORS();
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    CHECK_GL_ERRORS();
    glBindVertexArray(0);
    CHECK_GL_ERRORS();
}

void Mesh::draw_instanced(Shader& shader, uint32_t count) {
    CHECK_GL_ERRORS();
    std::size_t specular = 0;
    std::size_t diffuse = 0;
    for (std::size_t i = 0; i < textures.size(); ++i) {
        glActiveTexture(GL_TEXTURE0 + i);
        CHECK_GL_ERRORS();
        if (textures[i].type == Texture_Type::diffuse) {
            shader.set_int("material.texture_diffuse" + std::to_string(diffuse), i);
            ++diffuse;
        } else {
            shader.set_int("material.texture_specular" + std::to_string(specular), i);
            ++specular;
        }
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
        CHECK_GL_ERRORS();
    }
    CHECK_GL_ERRORS();
    glBindVertexArray(vao);
    CHECK_GL_ERRORS();
    glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0, count);
    CHECK_GL_ERRORS();
    glBindVertexArray(0);
    CHECK_GL_ERRORS();
}

void Mesh::bind() {
    CHECK_GL_ERRORS();
    glBindVertexArray(vao);
    CHECK_GL_ERRORS();
}
void Mesh::unbind() {
    CHECK_GL_ERRORS();
    glBindVertexArray(0);
    CHECK_GL_ERRORS();
}

void Mesh::prepare_mesh() {
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), &indices[0], GL_STATIC_DRAW);

    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    // normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(Vector3)));
    glEnableVertexAttribArray(1);
    // texture coordinates
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(2 * sizeof(Vector3)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}