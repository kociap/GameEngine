#include "gizmo.hpp"

#include "assets.hpp"
#include "containers/vector.hpp"
#include "debug_macros.hpp"
#include "gizmo_internal.hpp"
#include "glad/glad.h"
#include "math/math.hpp"
#include "math/quaternion.hpp"
#include "math/transform.hpp"
#include "opengl.hpp"
#include "shader.hpp"
#include "shader_file.hpp"
#include "time.hpp"
#include "utility.hpp"

void swap(gizmo::Vertex& a, gizmo::Vertex& b) {
    swap(a.position, b.position);
    swap(a.color, b.color);
}

namespace gizmo {
    struct Point_Draw_Data {
        Vector3 origin;
        float size;
        Color color;
    };

    struct Render_State {
        float lifetime;
        bool depth_test;
    };

    struct Line_Render_State {
        float lifetime;
        float width;
        bool depth_test;
    };

    static containers::Vector<Point_Draw_Data> point_draw_data;
    static containers::Vector<Vertex> line_vertices;
    static containers::Vector<containers::Vector<Vertex>> line_strips;
    static containers::Vector<containers::Vector<Vertex>> line_loops;
    static containers::Vector<Vertex> triangles;
    static containers::Vector<containers::Vector<Vertex>> triangle_strips;
    static containers::Vector<containers::Vector<Vertex>> triangle_fans;
    static containers::Vector<Render_State> point_render_state;
    static containers::Vector<Line_Render_State> line_render_state;
    static containers::Vector<Line_Render_State> line_strips_render_state;
    static containers::Vector<Line_Render_State> line_loops_render_state;
    static containers::Vector<Render_State> triangles_render_state;
    static containers::Vector<Render_State> triangle_strips_render_state;
    static containers::Vector<Render_State> triangle_fans_render_state;
    static uint32_t vertex_vao = 0;
    static uint32_t points_vao = 0;
    static uint32_t vertex_vbo = 0;
    static uint32_t points_vbo = 0;
    static uint32_t instanced_points_vbo = 0;
    static Shader line_shader = Shader(false);
    static Shader billboard_shader = Shader(false);

    void draw_point(Vector3 position, float size, Color color, float t, bool depth_test) {
        point_draw_data.emplace_back(position, size, color);
        point_render_state.push_back({t, depth_test});
    }

    void draw_line(Vector3 start, Vector3 end, Color color, float width, float t, bool depth_test) {
        GE_assert(width > 0.0f, "Line width may not be less than or equal 0");
        line_vertices.emplace_back(start, color);
        line_vertices.emplace_back(end, color);
        line_render_state.push_back({t, width, depth_test});
    }

    void draw_polyline(containers::Vector<Vertex> vertices, bool closed, float width, float t, bool depth_test) {
        GE_assert(width > 0.0f, "Line width may not be less than or equal 0");
        if (closed) {
            line_loops.push_back(std::move(vertices));
            line_loops_render_state.push_back({t, width, depth_test});
        } else {
            line_strips.push_back(std::move(vertices));
            line_strips_render_state.push_back({t, width, depth_test});
        }
    }

    void draw_triangle(Vector3 vert1, Vector3 vert2, Vector3 vert3, Color color, float t, bool depth_test) {
        triangles.emplace_back(vert1, color);
        triangles.emplace_back(vert2, color);
        triangles.emplace_back(vert3, color);
        triangles_render_state.push_back({t, depth_test});
    }

    void draw_cone(Vector3 position, Vector3 normal, float base_radius, uint32_t base_point_count, float height, Color color, float t, bool depth_test) {
        containers::Vector<Vertex> base_vertices(base_point_count);
        containers::Vector<Vertex> cone_vertices(base_point_count + 1);
        float angle = math::radians(360.0f / static_cast<float>(base_point_count));
        Vector3 rotation_axis = normal * math::sin(angle / 2.0f);
        Quaternion rotation_quat(rotation_axis.x, rotation_axis.y, rotation_axis.z, math::cos(angle / 2.0f));
        Vector3 vertex = {normal.y, -normal.x, 0};
        if (vertex.is_almost_zero()) {
            vertex = {0, normal.z, -normal.y};
        }
        vertex.normalize();
        Quaternion rotated_vec = Quaternion(vertex.x, vertex.y, vertex.z, 0);
        for (uint32_t i = 0; i < base_point_count; ++i) {
            rotated_vec = rotation_quat * rotated_vec * math::conjugate(rotation_quat);
            cone_vertices[i + 1] = base_vertices[i] = {Vector3(rotated_vec.x, rotated_vec.y, rotated_vec.z) * base_radius + position, color};
        }

        triangle_fans.push_back(std::move(base_vertices));
        triangle_fans_render_state.push_back({t, depth_test});

        cone_vertices[0] = {position + normal * height, color};
        triangle_fans.push_back(std::move(cone_vertices));
        triangle_fans_render_state.push_back({t, depth_test});
    }

    void draw_cube(Vector3 position, Vector3 forward, Vector3 right, Vector3 up, float size, Color color, float t, bool depth_test) {
        Vector3 half_x = right * size * 0.5f;
        Vector3 half_y = up * size * 0.5f;
        Vector3 half_z = forward * size * 0.5f;

        containers::Vector<Vertex> vertices(14);
        vertices[0] = {position - half_x - half_z - half_y, color};
        vertices[1] = {position + half_x - half_z - half_y, color};
        vertices[2] = {position - half_x + half_z - half_y, color};
        vertices[3] = {position + half_x + half_z - half_y, color};
        vertices[4] = {position - half_x + half_z + half_y, color};
        vertices[5] = {position + half_x + half_z + half_y, color};
        vertices[6] = {position + half_x - half_z - half_y, color};
        vertices[7] = {position + half_x - half_z + half_y, color};
        vertices[8] = {position - half_x - half_z - half_y, color};
        vertices[9] = {position - half_x - half_z + half_y, color};
        vertices[10] = {position - half_x + half_z - half_y, color};
        vertices[11] = {position - half_x + half_z + half_y, color};
        vertices[12] = {position + half_x - half_z + half_y, color};
        vertices[13] = {position + half_x + half_z + half_y, color};

        triangle_strips.push_back(std::move(vertices));
        triangle_strips_render_state.push_back({t, depth_test});
    }

    void draw_polygon(containers::Vector<Vertex> /* vertices */, float /* time */, bool /* depth_test */) {}

    void draw_circle(Vector3 position, Vector3 normal, float radius, Color color, uint32_t point_count, float line_width, float t, bool depth_test) {
        GE_assert(line_width > 0.0f, "Line width may not be less than or equal 0");

        containers::Vector<Vertex> vertices(point_count);
        float angle = math::radians(360.0f / static_cast<float>(point_count));
        Vector3 rotation_axis = normal * math::sin(angle / 2.0f);
        Quaternion rotation_quat(rotation_axis.x, rotation_axis.y, rotation_axis.z, math::cos(angle / 2.0f));
        Vector3 vertex = {normal.y, -normal.x, 0};
        if (vertex.is_almost_zero()) {
            vertex = {0, normal.z, -normal.y};
        }
        vertex.normalize();
        Quaternion rotated_vec = Quaternion(vertex.x, vertex.y, vertex.z, 0);
        for (uint32_t i = 0; i < point_count; ++i) {
            rotated_vec = rotation_quat * rotated_vec * math::conjugate(rotation_quat);
            vertices[i] = {Vector3(rotated_vec.x, rotated_vec.y, rotated_vec.z) * radius + position, color};
        }

        line_loops.push_back(std::move(vertices));
        line_loops_render_state.push_back({t, line_width, depth_test});
    }

    void draw_circle_filled(Vector3 position, Vector3 normal, float radius, Color color, uint32_t point_count, float t, bool depth_test) {
        containers::Vector<Vertex> vertices(point_count);
        float angle = math::radians(360.0f / static_cast<float>(point_count));
        Vector3 rotation_axis = normal * math::sin(angle / 2.0f);
        Quaternion rotation_quat(rotation_axis.x, rotation_axis.y, rotation_axis.z, math::cos(angle / 2.0f));
        Vector3 vertex = {normal.y, -normal.x, 0};
        if (vertex.is_almost_zero()) {
            vertex = {0, normal.z, -normal.y};
        }
        vertex.normalize();
        Quaternion rotated_vec = Quaternion(vertex.x, vertex.y, vertex.z, 0);
        for (uint32_t i = 0; i < point_count; ++i) {
            rotated_vec = rotation_quat * rotated_vec * math::conjugate(rotation_quat);
            vertices[i] = {Vector3(rotated_vec.x, rotated_vec.y, rotated_vec.z) * radius + position, color};
        }

        triangle_fans.push_back(std::move(vertices));
        triangle_fans_render_state.push_back({t, depth_test});
    }

    // internal

    void init() {
        Shader_File gizmo_line_vert = assets::load_shader_file("editor/gizmo_line.vert");
        Shader_File gizmo_line_frag = assets::load_shader_file("editor/gizmo_line.frag");
        line_shader = create_shader(gizmo_line_vert, gizmo_line_frag);

        Shader_File gizmo_billboard_vert = assets::load_shader_file("editor/gizmo_billboard.vert");
        Shader_File gizmo_billboard_frag = assets::load_shader_file("editor/gizmo_billboard.frag");
        billboard_shader = create_shader(gizmo_billboard_vert, gizmo_billboard_frag);

        opengl::gen_vertex_arrays(1, &vertex_vao);
        opengl::bind_vertex_array(vertex_vao);
        opengl::gen_buffers(1, &vertex_vbo);
        opengl::bind_buffer(opengl::Buffer_Type::array_buffer, vertex_vbo);
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
        glDeleteVertexArrays(1, &vertex_vao);
        glDeleteVertexArrays(1, &points_vao);
        glDeleteBuffers(1, &vertex_vbo);
        glDeleteBuffers(1, &points_vbo);
        glDeleteBuffers(1, &instanced_points_vbo);
    }

    void update() {
        float delta_time = Timef::get_unscaled_delta_time();
        for (containers::Vector<float>::size_type i = 0; i < line_render_state.size(); ++i) {
            if (line_render_state[i].lifetime <= 0) {
                line_render_state.erase_unsorted_unchecked(i);
                // Erase in this particular order to preserve order of other vertices
                line_vertices.erase_unsorted_unchecked(2 * i + 1);
                line_vertices.erase_unsorted_unchecked(2 * i);
                --i;
            } else {
                line_render_state[i].lifetime -= delta_time;
            }
        }

        for (containers::Vector<float>::size_type i = 0; i < line_loops.size(); ++i) {
            if (line_loops_render_state[i].lifetime <= 0) {
                line_loops_render_state.erase_unsorted_unchecked(i);
                line_loops.erase_unsorted_unchecked(i);
                --i;
            } else {
                line_loops_render_state[i].lifetime -= delta_time;
            }
        }

        for (containers::Vector<float>::size_type i = 0; i < point_render_state.size(); ++i) {
            if (point_render_state[i].lifetime <= 0) {
                point_render_state.erase_unsorted_unchecked(i);
                point_draw_data.erase_unsorted_unchecked(i);
                --i;
            } else {
                point_render_state[i].lifetime -= delta_time;
            }
        }

        for (containers::Vector<float>::size_type i = 0; i < triangle_strips.size(); ++i) {
            if (triangle_strips_render_state[i].lifetime <= 0) {
                triangle_strips_render_state.erase_unsorted_unchecked(i);
                triangle_strips.erase_unsorted_unchecked(i);
                --i;
            } else {
                triangle_strips_render_state[i].lifetime -= delta_time;
            }
        }

        for (containers::Vector<float>::size_type i = 0; i < triangle_fans.size(); ++i) {
            if (triangle_fans_render_state[i].lifetime <= 0) {
                triangle_fans_render_state.erase_unsorted_unchecked(i);
                triangle_fans.erase_unsorted_unchecked(i);
                --i;
            } else {
                triangle_fans_render_state[i].lifetime -= delta_time;
            }
        }
    }

    static Matrix4 compute_billboard_rotation_matrix(Vector3 billboard_look_dir, Vector3 camera_up) {
        Vector3 billboard_right = math::cross(camera_up, billboard_look_dir);
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
            containers::Vector<Quad_Instance_Data> quad_instance_data(point_draw_data.size(), containers::reserve);
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

        glDisable(GL_CULL_FACE);

        if (line_vertices.size() > 0 || line_loops.size() > 0 || triangles.size() > 0 || triangle_strips.size() > 0 || triangle_fans.size() > 0) {
            line_shader.use();
            line_shader.set_matrix4("view", view);
            line_shader.set_matrix4("projection", projection);
            opengl::bind_vertex_array(vertex_vao);
            opengl::bind_buffer(opengl::Buffer_Type::array_buffer, vertex_vbo);
        }

        if (line_vertices.size() > 0) {
            uint64_t depth_enabled_end = 0;
            for (uint64_t k = 0; k < line_render_state.size(); ++k) {
                if (line_render_state[k].depth_test) {
                    if (k != depth_enabled_end) {
                        swap(line_render_state[k], line_render_state[depth_enabled_end]);
                        swap(line_vertices[2 * k + 1], line_vertices[2 * depth_enabled_end + 1]);
                        swap(line_vertices[2 * k], line_vertices[2 * depth_enabled_end]);
                    }
                    ++depth_enabled_end;
                }
            }
            opengl::buffer_data(opengl::Buffer_Type::array_buffer, line_vertices.size() * sizeof(Vertex), line_vertices.data(), GL_STREAM_DRAW);
            // Autistic screeching
            glEnable(GL_DEPTH_TEST);
            for (uint64_t i = 0; i < depth_enabled_end; ++i) {
                glLineWidth(line_render_state[i].width);
                opengl::draw_arrays(GL_LINES, 2 * i, 2);
            }
            glDisable(GL_DEPTH_TEST);
            for (uint64_t i = depth_enabled_end; i < line_render_state.size(); ++i) {
                glLineWidth(line_render_state[i].width);
                opengl::draw_arrays(GL_LINES, 2 * i, 2);
            }
        }

        if (line_loops.size() > 0) {
            // More autistic screeching
            for (uint64_t i = 0; i < line_loops.size(); ++i) {
                if (line_loops_render_state[i].depth_test) {
                    glEnable(GL_DEPTH_TEST);
                } else {
                    glDisable(GL_DEPTH_TEST);
                }

                glLineWidth(line_loops_render_state[i].width);
                opengl::buffer_data(opengl::Buffer_Type::array_buffer, line_loops[i].size() * sizeof(Vertex), line_loops[i].data(), GL_STREAM_DRAW);
                opengl::draw_arrays(GL_LINE_LOOP, 0, line_loops[i].size());
            }
        }

        if (triangles.size() > 0) {
            opengl::buffer_data(opengl::Buffer_Type::array_buffer, triangles.size() * sizeof(Vertex), triangles.data(), GL_STREAM_DRAW);
            opengl::draw_arrays(GL_TRIANGLES, 0, triangles.size());
        }

        if (triangle_strips.size() > 0) {
            // Autistic screeching intensifies beyond intensity
            for (uint64_t i = 0; i < triangle_strips.size(); ++i) {
                if (triangle_strips_render_state[i].depth_test) {
                    glEnable(GL_DEPTH_TEST);
                } else {
                    glDisable(GL_DEPTH_TEST);
                }

                opengl::buffer_data(opengl::Buffer_Type::array_buffer, triangle_strips[i].size() * sizeof(Vertex), triangle_strips[i].data(), GL_STREAM_DRAW);
                opengl::draw_arrays(GL_TRIANGLE_STRIP, 0, triangle_strips[i].size());
            }
        }

        if (triangle_fans.size() > 0) {
            // Autistic screeching intensifies beyond intensity
            for (uint64_t i = 0; i < triangle_fans.size(); ++i) {
                if (triangle_fans_render_state[i].depth_test) {
                    glEnable(GL_DEPTH_TEST);
                } else {
                    glDisable(GL_DEPTH_TEST);
                }

                opengl::buffer_data(opengl::Buffer_Type::array_buffer, triangle_fans[i].size() * sizeof(Vertex), triangle_fans[i].data(), GL_STREAM_DRAW);
                opengl::draw_arrays(GL_TRIANGLE_FAN, 0, triangle_fans[i].size());
            }
        }

        opengl::bind_vertex_array(0);
        glLineWidth(1);
        glEnable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
    }
} // namespace gizmo