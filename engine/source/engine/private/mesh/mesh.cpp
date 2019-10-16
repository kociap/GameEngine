#include <mesh/mesh.hpp>

#include <glad.hpp>
#include <opengl.hpp>
#include <renderer.hpp>
#include <shader.hpp>

namespace anton_engine {
    Vertex::Vertex(Vector3 pos, Vector3 norm, Vector3 tan, Vector3 bitan, Vector2 tex)
        : position(std::move(pos)), normal(std::move(norm)), tangent(std::move(tan)), bitangent(std::move(bitan)), uv_coordinates(std::move(tex)) {}

    Mesh::Mesh() {}

    Mesh::Mesh(anton_stl::Vector<Vertex> const& vertices, anton_stl::Vector<uint32_t> const& indices): vertices(vertices), indices(indices) {
        prepare_mesh();
    }

    Mesh::Mesh(anton_stl::Vector<Vertex>&& vertices, anton_stl::Vector<uint32_t>&& indices): vertices(std::move(vertices)), indices(std::move(indices)) {
        prepare_mesh();
    }

    Mesh::Mesh(Mesh&& from) noexcept: vertices(std::move(from.vertices)), indices(std::move(from.indices)) {
        std::swap(ebo, from.ebo);
        std::swap(vbo, from.vbo);
    }

    Mesh& Mesh::operator=(Mesh&& from) noexcept {
        vertices = std::move(from.vertices);
        indices = std::move(from.indices);
        std::swap(ebo, from.ebo);
        std::swap(vbo, from.vbo);
        return *this;
    }

    Mesh::~Mesh() {
        if (vbo) {
            glDeleteBuffers(1, &vbo);
        }
        if (ebo) {
            glDeleteBuffers(1, &ebo);
        }
    }

    uint32_t Mesh::get_vbo() const {
        return vbo;
    }

    uint32_t Mesh::get_ebo() const {
        return ebo;
    }

    void Mesh::prepare_mesh() {
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferStorage(GL_ARRAY_BUFFER, vertices.size() * static_cast<anton_stl::ssize_t>(sizeof(Vertex)), vertices.data(), 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        // Multiply by 4 because each index is 4 bytes
        glBufferStorage(GL_ELEMENT_ARRAY_BUFFER, indices.size() * 4, indices.data(), 0);
    }
} // namespace anton_engine
