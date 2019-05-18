#include "gizmo.hpp"

#include "assets.hpp"
#include "containers/vector.hpp"
#include "gizmo_internal.hpp"
#include "glad/glad.h"
#include "opengl.hpp"
#include "shader.hpp"
#include "shader_file.hpp"
#include "time.hpp"

namespace gizmo {
    struct Line_Vertex {
        Vector3 position;
        Color color;
    };

    static containers::Vector<Line_Vertex> line_vertices;
    static containers::Vector<float> line_lifetimes;
    static uint32_t lines_vao = 0;
    static uint32_t lines_vbo = 0;
    static Shader line_shader = Shader(false);

    void draw_line(Vector3 start, Vector3 end, Color color, float t) {
        line_vertices.emplace_back(start, color);
        line_vertices.emplace_back(end, color);
        line_lifetimes.push_back(t);
    }

    // internal

    void init() {
        Shader_File gizmo_line_vert = assets::load_shader_file("editor/gizmo_line.vert");
        Shader_File gizmo_line_frag = assets::load_shader_file("editor/gizmo_line.frag");
        line_shader = create_shader(gizmo_line_vert, gizmo_line_frag);

        opengl::gen_vertex_arrays(1, &lines_vao);
        opengl::bind_vertex_array(lines_vao);
        opengl::gen_buffers(1, &lines_vbo);
        opengl::bind_buffer(opengl::Buffer_Type::array_buffer, lines_vbo);
        opengl::vertex_array_attribute(0, 3, GL_FLOAT, sizeof(Line_Vertex), offsetof(Line_Vertex, position));
        opengl::enable_vertex_array_attribute(0);
        opengl::vertex_array_attribute(1, 4, GL_FLOAT, sizeof(Line_Vertex), offsetof(Line_Vertex, color));
        opengl::enable_vertex_array_attribute(1);
    }

    void terminate() {
        delete_shader(line_shader);
        glDeleteVertexArrays(1, &lines_vao);
        glDeleteBuffers(1, &lines_vbo);
    }

    void update() {
        float delta_time = Timef::get_unscaled_delta_time();
        for (containers::Vector<float>::size_type i = 0; i < line_lifetimes.size(); ++i) {
            if (line_lifetimes[i] <= 0) {
                line_lifetimes.erase_unsorted_unchecked(i);
                // Erase in this particular order to preserve order of other vertices
                line_vertices.erase_unsorted_unchecked(2 * i + 1);
                line_vertices.erase_unsorted_unchecked(2 * i);
                --i;
            } else {
                line_lifetimes[i] -= delta_time;
            }
        }
    }

    void draw(Matrix4 view, Matrix4 projection) {
        if (line_vertices.size() > 0) {
            line_shader.use();
            line_shader.set_matrix4("view", view);
            line_shader.set_matrix4("projection", projection);
            opengl::bind_vertex_array(lines_vao);
            opengl::bind_buffer(opengl::Buffer_Type::array_buffer, lines_vbo);
            opengl::buffer_data(opengl::Buffer_Type::array_buffer, line_vertices.size() * sizeof(Line_Vertex), line_vertices.data(), GL_STREAM_DRAW);
            opengl::draw_arrays(GL_LINES, 0, line_vertices.size());
        }
    }
} // namespace gizmo