#include "gizmo.hpp"

#include "assets.hpp"
#include "containers/vector.hpp"
#include "gizmo_internal.hpp"
#include "glad/glad.h"
#include "math/transform.hpp"
#include "opengl.hpp"
#include "shader.hpp"
#include "shader_file.hpp"
#include "time.hpp"

namespace gizmo {
    struct Vertex {
        Vector3 position;
        Color color;
    };

    struct Point_Draw_Data {
        Vector3 origin;
        float size;
        Color color;
    };

    static containers::Vector<Vertex> line_vertices;
    static containers::Vector<float> line_lifetimes;
    static containers::Vector<Point_Draw_Data> point_draw_data;
    static containers::Vector<float> point_lifetimes;
    static uint32_t lines_vao = 0;
    static uint32_t points_vao = 0;
    static uint32_t lines_vbo = 0;
    static uint32_t points_vbo = 0;
    static uint32_t instanced_points_vbo = 0;
    static Shader line_shader = Shader(false);
    static Shader billboard_shader = Shader(false);

    void draw_line(Vector3 start, Vector3 end, Color color, float t) {
        line_vertices.emplace_back(start, color);
        line_vertices.emplace_back(end, color);
        line_lifetimes.push_back(t);
    }

    void draw_point(Vector3 position, float size, Color color, float t) {
        point_draw_data.emplace_back(position, size, color);
        point_lifetimes.push_back(t);
    }

    void draw_fixed_size_point(Vector3 position, float size, Color color, float t) {}

    // internal

    void init() {
        Shader_File gizmo_line_vert = assets::load_shader_file("editor/gizmo_line.vert");
        Shader_File gizmo_line_frag = assets::load_shader_file("editor/gizmo_line.frag");
        line_shader = create_shader(gizmo_line_vert, gizmo_line_frag);

        Shader_File gizmo_billboard_vert = assets::load_shader_file("editor/gizmo_billboard.vert");
        Shader_File gizmo_billboard_frag = assets::load_shader_file("editor/gizmo_billboard.frag");
        billboard_shader = create_shader(gizmo_billboard_vert, gizmo_billboard_frag);

        opengl::gen_vertex_arrays(1, &lines_vao);
        opengl::bind_vertex_array(lines_vao);
        opengl::gen_buffers(1, &lines_vbo);
        opengl::bind_buffer(opengl::Buffer_Type::array_buffer, lines_vbo);
        opengl::vertex_array_attribute(0, 3, GL_FLOAT, sizeof(Vertex), offsetof(Vertex, position));
        opengl::enable_vertex_array_attribute(0);
        opengl::vertex_array_attribute(1, 4, GL_FLOAT, sizeof(Vertex), offsetof(Vertex, color));
        opengl::enable_vertex_array_attribute(1);

        // clang-format off
        static Vector3 quad_vertex_positions[6] = {
            {-0.5f,  0.5f,  0.0f},
            { 0.5f, -0.5f,  0.0f},
            { 0.5f,  0.5f,  0.0f},
            {-0.5f,  0.5f,  0.0f},
            {-0.5f, -0.5f,  0.0f},
            { 0.5f, -0.5f,  0.0f}
        };
        // clang-format on

        opengl::gen_vertex_arrays(1, &points_vao);
        opengl::bind_vertex_array(points_vao);

        opengl::gen_buffers(1, &points_vbo);
        opengl::bind_buffer(opengl::Buffer_Type::array_buffer, points_vbo);
        opengl::buffer_data(opengl::Buffer_Type::array_buffer, 6 * sizeof(Vector3), quad_vertex_positions, GL_STATIC_DRAW);
        // Position
        opengl::vertex_array_attribute(0, 3, GL_FLOAT, sizeof(Vector3), 0);
        opengl::enable_vertex_array_attribute(0);

        opengl::gen_buffers(1, &instanced_points_vbo);
        opengl::bind_buffer(opengl::Buffer_Type::array_buffer, instanced_points_vbo);
        // Model matrix
        opengl::vertex_array_attribute(1, 4, GL_FLOAT, sizeof(Color) + sizeof(Matrix4), 0);
        opengl::enable_vertex_array_attribute(1);
        glVertexAttribDivisor(1, 1);
        opengl::vertex_array_attribute(2, 4, GL_FLOAT, sizeof(Color) + sizeof(Matrix4), 4 * sizeof(float));
        opengl::enable_vertex_array_attribute(2);
        glVertexAttribDivisor(2, 1);
        opengl::vertex_array_attribute(3, 4, GL_FLOAT, sizeof(Color) + sizeof(Matrix4), 8 * sizeof(float));
        opengl::enable_vertex_array_attribute(3);
        glVertexAttribDivisor(3, 1);
        opengl::vertex_array_attribute(4, 4, GL_FLOAT, sizeof(Color) + sizeof(Matrix4), 12 * sizeof(float));
        opengl::enable_vertex_array_attribute(4);
        glVertexAttribDivisor(4, 1);
        // Color
        opengl::vertex_array_attribute(5, 4, GL_FLOAT, sizeof(Color) + sizeof(Matrix4), sizeof(Matrix4));
        opengl::enable_vertex_array_attribute(5);
        glVertexAttribDivisor(5, 1);
    }

    void terminate() {
        delete_shader(line_shader);
        delete_shader(billboard_shader);
        glDeleteVertexArrays(1, &lines_vao);
        glDeleteVertexArrays(1, &points_vao);
        glDeleteBuffers(1, &lines_vbo);
        glDeleteBuffers(1, &points_vbo);
        glDeleteBuffers(1, &instanced_points_vbo);
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

        for (containers::Vector<float>::size_type i = 0; i < point_lifetimes.size(); ++i) {
            if (point_lifetimes[i] <= 0) {
                point_lifetimes.erase_unsorted_unchecked(i);
                point_draw_data.erase_unsorted_unchecked(i);
                --i;
            } else {
                point_lifetimes[i] -= delta_time;
            }
        }
    }

    static Matrix4 compute_billboard_rotation_matrix(Vector3 billboard_look_dir, Vector3 camera_up) {
        Vector3 billboard_right = math::normalize(math::cross(camera_up, billboard_look_dir));
        Vector3 billboard_up = math::cross(billboard_look_dir, billboard_right);
        return {Vector4{billboard_right, 0}, Vector4{billboard_up, 0}, Vector4{billboard_look_dir, 0}, {0, 0, 0, 1}};
    }

    void draw(Vector3 camera_position, Matrix4 view, Matrix4 projection) {
        if (point_draw_data.size() > 0) {
            billboard_shader.use();
            line_shader.set_matrix4("projection", projection);

            struct Quad_Instance_Data {
                Matrix4 mat;
                Color color;
            };
            containers::Vector<Quad_Instance_Data> quad_instance_data;
            quad_instance_data.reserve(point_draw_data.size());
            for (containers::Vector<Point_Draw_Data>::size_type i = 0; i < point_draw_data.size(); ++i) {
                Point_Draw_Data pdd = point_draw_data[i];
                Vector3 direction = math::normalize(camera_position - pdd.origin);
                Vector3 camera_up = {view(0, 1), view(1, 1), view(2, 1)};
                Matrix4 rotation_mat = compute_billboard_rotation_matrix(direction, camera_up);
                Matrix4 mv_mat = math::transform::scale(pdd.size) * rotation_mat * math::transform::translate(pdd.origin) * view;
                quad_instance_data.emplace_back(mv_mat, pdd.color);
            }
            opengl::bind_buffer(opengl::Buffer_Type::array_buffer, instanced_points_vbo);
            opengl::buffer_data(opengl::Buffer_Type::array_buffer, quad_instance_data.size() * sizeof(Quad_Instance_Data), quad_instance_data.data(),
                                GL_STREAM_DRAW);
            opengl::bind_vertex_array(points_vao);
            // count is 6 since we are rendering a quad (2x triangle)
            glDrawArraysInstanced(GL_TRIANGLES, 0, 6, point_draw_data.size());
        }

        if (line_vertices.size() > 0) {
            line_shader.use();
            line_shader.set_matrix4("view", view);
            line_shader.set_matrix4("projection", projection);
            opengl::bind_vertex_array(lines_vao);
            opengl::bind_buffer(opengl::Buffer_Type::array_buffer, lines_vbo);
            opengl::buffer_data(opengl::Buffer_Type::array_buffer, line_vertices.size() * sizeof(Vertex), line_vertices.data(), GL_STREAM_DRAW);
            opengl::draw_arrays(GL_LINES, 0, line_vertices.size());
        }
    }
} // namespace gizmo