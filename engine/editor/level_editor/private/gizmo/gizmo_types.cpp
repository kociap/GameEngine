#include <gizmo/arrow_3d.hpp>
#include <gizmo/dial_3d.hpp>
#include <gizmo/primitive_3d.hpp>

#include <anton_stl/vector.hpp>
#include <builtin_shaders.hpp>
#include <gizmo/gizmo.hpp>
#include <glad.hpp>
#include <intrinsics.hpp>
#include <math/math.hpp>
#include <math/quaternion.hpp>
#include <math/transform.hpp>
#include <math/vector3.hpp>
#include <obb.hpp>

#include <debug_macros.hpp> // CHECK_GL_ERRORS

namespace anton_engine::gizmo {
    constexpr i32 cone_base_point_count = 16;
    constexpr i32 cone_vertex_count = 2 * (cone_base_point_count + 2); // 2 * (circle with 1 duplicate + tip)
    constexpr i32 cube_vertex_count = 14;
    constexpr i32 dial_vertex_count = 32;

    struct Plane_Vertex_Data {
        Vector3 vertices[6];
    };

    struct Arrow_Vertex_Data {
        Vector3 cone[2 + cone_vertex_count];
        Vector3 cube[2 + cube_vertex_count];
    };

    struct Dial_Vertex_Data {
        Vector3 vertices[2 * dial_vertex_count + 2];
        Vector3 normals[2 * dial_vertex_count + 2];
        float scale_factors[2 * dial_vertex_count + 2];
    };

    struct Intersection_Meshes {
        Vector3 vertices[4 * 12];
        Vector3 normals[4 * 12];
        float scale_factors[4 * 12];
    };

    struct Gizmos_Vertex_Data {
        Plane_Vertex_Data plane;
        Arrow_Vertex_Data arrow;
        Dial_Vertex_Data dial;
        Intersection_Meshes intersection_meshes;
    };

    constexpr i32 plane_base_index = 0;
    constexpr i32 cone_base_index = 6;
    constexpr i32 cube_base_index = 6 + 2 + cone_vertex_count;

    // side size = 1
    // plane normal {0, 0, 1}
    static void generate_plane_geometry(Vector3* const vertices) {
        vertices[0] = {-0.5f, 0.5f, 0.0f};
        vertices[1] = {-0.5f, -0.5f, 0.0f};
        vertices[2] = {0.5f, -0.5f, 0.0f};
        vertices[3] = {0.5f, -0.5f, 0.0f};
        vertices[4] = {0.5f, 0.5f, 0.0f};
        vertices[5] = {-0.5f, 0.5f, 0.0f};
    }

    static void generate_cone_geometry(Vector3* const vertices) {
        vertices[0] = {0, 0, 0};
        vertices[1] = {0, 0, -0.8f};

        float angle = math::radians(360.0f / static_cast<float>(cone_base_point_count));
        Vector3 rotation_axis = Vector3::forward * math::sin(angle / 2.0f);
        Quaternion rotation_quat(rotation_axis.x, rotation_axis.y, rotation_axis.z, math::cos(angle / 2.0f));

        Vector3 vertex = {0, 0.05f, 0};
        Quaternion rotated_vec = Quaternion(vertex.x, vertex.y, vertex.z, 0);
        Vector3* const base = vertices + 2 + cone_base_point_count + 1;
        Vector3* const cone = vertices + 2;
        base[0] = {0, 0, -0.8f};
        cone[0] = {0, 0, -1.0f};
        for (i32 i = 0; i <= cone_base_point_count; ++i) {
            rotated_vec = rotation_quat * rotated_vec * math::conjugate(rotation_quat);
            base[1 + cone_base_point_count - i] = Vector3{rotated_vec.x, rotated_vec.y, -0.8f};
            cone[1 + i] = Vector3{rotated_vec.x, rotated_vec.y, -0.8f};
        }
    }

    static void generate_cube_geometry(Vector3* const vertices) {
        constexpr float half_size = 0.05f;
        Vector3 const offset = {0, 0, -1.0f + half_size};
        vertices[0] = {0, 0, 0};
        vertices[1] = {0, 0, -1.0f + half_size * 2};
        vertices[2] = offset + Vector3{half_size, -half_size, half_size};
        vertices[3] = offset + Vector3{-half_size, -half_size, half_size};
        vertices[4] = offset + Vector3{half_size, -half_size, -half_size};
        vertices[5] = offset + Vector3{-half_size, -half_size, -half_size};
        vertices[6] = offset + Vector3{-half_size, half_size, -half_size};
        vertices[7] = offset + Vector3{-half_size, -half_size, half_size};
        vertices[8] = offset + Vector3{-half_size, half_size, half_size};
        vertices[9] = offset + Vector3{half_size, -half_size, half_size};
        vertices[10] = offset + Vector3{half_size, half_size, half_size};
        vertices[11] = offset + Vector3{half_size, -half_size, -half_size};
        vertices[12] = offset + Vector3{half_size, half_size, -half_size};
        vertices[13] = offset + Vector3{-half_size, half_size, -half_size};
        vertices[14] = offset + Vector3{half_size, half_size, half_size};
        vertices[15] = offset + Vector3{-half_size, half_size, half_size};
    }

    static void generate_dial_geometry(Dial_Vertex_Data* const dial) {
        float const angle = math::constants::pi / dial_vertex_count;
        for (i32 i = 0; i <= 2 * dial_vertex_count; i += 2) {
            Vector3 const pos = {math::cos(angle * i), math::sin(angle * i), 0};
            dial->vertices[i] = pos;
            dial->vertices[i + 1] = pos;
        }

        for (i32 i = 2; i <= 2 * dial_vertex_count - 2; i += 2) {
            // normal = (vert[i] - vert[i - 2]) + (vert[i + 2] - vert[i])
            Vector3 const normal = math::normalize(dial->vertices[i + 2] - dial->vertices[i - 2]);
            dial->normals[i] = normal;
            dial->normals[i + 1] = normal;

            Vector3 const tangent = Vector3{-normal.y, normal.x, 0};
            Vector3 const line_segment = math::normalize(dial->vertices[i] - dial->vertices[i - 2]);
            float const factor = 1.0f / math::abs(math::dot(Vector3{-line_segment.y, line_segment.x, 0}, tangent));
            dial->scale_factors[i] = factor;
            dial->scale_factors[i + 1] = factor;
        }

        // First and last vertices are duplicates
        Vector3 const normal = math::normalize(dial->vertices[2] - dial->vertices[2 * dial_vertex_count - 2]);
        dial->normals[0] = normal;
        dial->normals[1] = normal;
        dial->normals[2 * dial_vertex_count] = normal;
        dial->normals[2 * dial_vertex_count + 1] = normal;

        Vector3 const line_segment = math::normalize(dial->vertices[0] - dial->vertices[2 * dial_vertex_count - 2]);
        Vector3 const tangent = Vector3{-normal.y, normal.x, 0};
        float const factor = 1 / math::abs(math::dot(Vector3{-line_segment.y, line_segment.x, 0}, tangent));
        dial->scale_factors[0] = factor;
        dial->scale_factors[1] = factor;
        dial->scale_factors[2 * dial_vertex_count] = factor;
        dial->scale_factors[2 * dial_vertex_count + 1] = factor;
    }

    // TODO: Local buffers to make this work
    static u32 vbo = 0;
    static u32 vao = 0;
    static u32 line_vao = 0;

    static void initialize_ogl() {
        Gizmos_Vertex_Data gizmos_vertex_data;
        generate_plane_geometry(gizmos_vertex_data.plane.vertices);
        generate_cone_geometry(gizmos_vertex_data.arrow.cone);
        generate_cube_geometry(gizmos_vertex_data.arrow.cube);
        generate_dial_geometry(&gizmos_vertex_data.dial);

        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferStorage(GL_ARRAY_BUFFER, sizeof(Gizmos_Vertex_Data) + 10000 * (sizeof(Vector3) + sizeof(Vector3) + sizeof(float)), &gizmos_vertex_data,
                        GL_DYNAMIC_STORAGE_BIT);

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        glEnableVertexAttribArray(0);
        glVertexAttribFormat(0, 3, GL_FLOAT, GL_FALSE, 0);
        glVertexAttribBinding(0, 0);
        glBindVertexBuffer(0, vbo, 0, sizeof(Vector3));

        glGenVertexArrays(1, &line_vao);
        glBindVertexArray(line_vao);
        glEnableVertexAttribArray(0);
        glVertexAttribFormat(0, 3, GL_FLOAT, false, 0);
        glVertexAttribBinding(0, 0);
        glEnableVertexAttribArray(1);
        glVertexAttribFormat(1, 3, GL_FLOAT, false, 0);
        glVertexAttribBinding(1, 1);
        glEnableVertexAttribArray(2);
        glVertexAttribFormat(2, 1, GL_FLOAT, false, 0);
        glVertexAttribBinding(2, 2);
        u64 const base_offset = offsetof(Gizmos_Vertex_Data, dial);
        glBindVertexBuffer(0, vbo, base_offset + offsetof(Dial_Vertex_Data, vertices), sizeof(Vector3));
        glBindVertexBuffer(1, vbo, base_offset + offsetof(Dial_Vertex_Data, normals), sizeof(Vector3));
        glBindVertexBuffer(2, vbo, base_offset + offsetof(Dial_Vertex_Data, scale_factors), sizeof(float));
    }

    struct Line_Segment {
        Vector3 begin;
        Vector3 end;
        float time;
    };

    static anton_stl::Vector<Line_Segment> line_segments;

    void debug_draw_line(Vector3 f, Vector3 l, float time) {
        line_segments.push_back({f, l, time});
    }

    void debug_commit_draw_lines(Matrix4 vp_mat, Vector3 camera_pos, float delta_time) {
        struct Line_Data {
            Vector3 verts[10000];
            Vector3 rotation_axes[10000];
            float scale_factors[10000];
        } lines;

        for (i32 i = 0; i < line_segments.size(); ++i) {
            lines.verts[4 * i] = lines.verts[4 * i + 1] = line_segments[i].begin;
            lines.verts[4 * i + 2] = lines.verts[4 * i + 3] = line_segments[i].end;
            lines.rotation_axes[4 * i] = lines.rotation_axes[4 * i + 1] = lines.rotation_axes[4 * i + 2] = lines.rotation_axes[4 * i + 3] =
                math::normalize(line_segments[i].end - line_segments[i].begin);
            lines.scale_factors[4 * i] = lines.scale_factors[4 * i + 1] = lines.scale_factors[4 * i + 2] = lines.scale_factors[4 * i + 3] = 1.0f;
        }

        if (ANTON_UNLIKELY(vao == 0)) {
            initialize_ogl();
        }

        Shader& uniform_color_shader = get_builtin_shader(Builtin_Shader::uniform_color_line_3d);
        uniform_color_shader.use();
        uniform_color_shader.set_matrix4("model_mat", Matrix4::identity);
        uniform_color_shader.set_matrix4("vp_mat", vp_mat);
        uniform_color_shader.set_vec4("color", Color::green);
        uniform_color_shader.set_vec3("camera_pos", camera_pos);
        uniform_color_shader.set_float("line_width", 0.018f);

        glNamedBufferSubData(vbo, sizeof(Gizmos_Vertex_Data), sizeof(Line_Data), &lines);
        glBindVertexArray(line_vao);
        u64 const base_offset = sizeof(Gizmos_Vertex_Data);
        glBindVertexBuffer(0, vbo, base_offset + offsetof(Line_Data, verts), sizeof(Vector3));
        glBindVertexBuffer(1, vbo, base_offset + offsetof(Line_Data, rotation_axes), sizeof(Vector3));
        glBindVertexBuffer(2, vbo, base_offset + offsetof(Line_Data, scale_factors), sizeof(float));
        i32 offsets[10000];
        i32 counts[10000];
        for (i32 i = 0; i < line_segments.size(); ++i) {
            offsets[i] = 4 * i;
            counts[i] = 4;
        }
        glMultiDrawArrays(GL_TRIANGLE_STRIP, offsets, counts, line_segments.size());

        for (isize i = 0; i < line_segments.size(); ++i) {
            Line_Segment& line = line_segments[i];
            line.time -= delta_time;
            if (line.time < 0.0f) {
                line_segments.erase_unsorted_unchecked(i);
                --i;
            }
        }
    }

    // void draw_primitive_3d(Primitive_3D const primitive, Matrix4 const world_transform, Matrix4 const view_projection_matrix, Vector2 const viewport_size) {
    //     if (ANTON_UNLIKELY(vao == 0)) {
    //         initialize_ogl();
    //     }

    // }

    static int write_intersection_geometry_data_to_gpu() {
        Intersection_Meshes meshes;
        auto create_line = [&meshes](i32 index, Vector3 begin, Vector3 end) {
            meshes.vertices[4 * index] = meshes.vertices[4 * index + 1] = begin;
            meshes.vertices[4 * index + 2] = meshes.vertices[4 * index + 3] = end;
        };

        create_line(0, {-0.05f, 0.05f, 0.0f}, {-0.05f, 0.05f, -0.8f});
        create_line(1, {0.05f, 0.05f, 0.0f}, {0.05f, 0.05f, -0.8f});
        create_line(2, {-0.05f, -0.05f, 0.0f}, {-0.05f, -0.05f, -0.8f});
        create_line(3, {0.05f, -0.05f, 0.0f}, {0.05f, -0.05f, -0.8f});

        create_line(4, {-0.05f, 0.05f, 0.0f}, {0.05f, 0.05f, 0.0f});
        create_line(5, {-0.05f, -0.05f, 0.0f}, {0.05f, -0.05f, 0.0f});
        create_line(6, {-0.05f, 0.05f, -0.8f}, {0.05f, 0.05f, -0.8f});
        create_line(7, {-0.05f, -0.05f, -0.8f}, {0.05f, -0.05f, -0.8f});

        create_line(8, {-0.05f, -0.05f, 0.0f}, {-0.05f, 0.05f, 0.0f});
        create_line(9, {0.05f, -0.05f, 0.0f}, {0.05f, 0.05f, 0.0f});
        create_line(10, {-0.05f, -0.05f, -0.8f}, {-0.05f, 0.05f, -0.8f});
        create_line(11, {0.05f, -0.05f, -0.8f}, {0.05f, 0.05f, -0.8f});

        for (i32 i = 0; i < 16; ++i) {
            meshes.normals[i] = Vector3::forward;
            meshes.normals[i + 16] = Vector3::right;
            meshes.normals[i + 32] = Vector3::up;
        }

        for (i32 i = 0; i < 48; ++i) {
            meshes.scale_factors[i] = 1.0f;
        }

        glNamedBufferSubData(vbo, offsetof(Gizmos_Vertex_Data, intersection_meshes), sizeof(Intersection_Meshes), &meshes);

        return 0;
    }

    void draw_arrow_3d(Arrow_3D const arrow, Matrix4 const world_transform, Matrix4 const view_projection_matrix, Vector2 const viewport_size,
                       Vector3 camera_pos) {
        // TODO: For debugging purposes
        {
            if (vao == 0) {
                initialize_ogl();
            }

            static int xyz_call_this_function_once = write_intersection_geometry_data_to_gpu();

            Shader& uniform_color_shader = get_builtin_shader(Builtin_Shader::uniform_color_line_3d);
            uniform_color_shader.use();
            Matrix4 const vp_mat = view_projection_matrix;
            float scale = compute_scale(world_transform, arrow.size, vp_mat, viewport_size);
            uniform_color_shader.set_matrix4("model_mat", math::transform::scale(scale) * world_transform);
            uniform_color_shader.set_matrix4("vp_mat", vp_mat);
            uniform_color_shader.set_vec4("color", Color(1.0f, 147.0f / 255.0f, 1.0f / 255.0f));
            uniform_color_shader.set_vec3("camera_pos", camera_pos);
            uniform_color_shader.set_float("line_width", scale * 0.01f);
            glBindVertexArray(line_vao);
            u64 const base_offset = offsetof(Gizmos_Vertex_Data, intersection_meshes);
            glBindVertexBuffer(0, vbo, base_offset + offsetof(Intersection_Meshes, vertices), sizeof(Vector3));
            glBindVertexBuffer(1, vbo, base_offset + offsetof(Intersection_Meshes, normals), sizeof(Vector3));
            glBindVertexBuffer(2, vbo, base_offset + offsetof(Intersection_Meshes, scale_factors), sizeof(float));
            for (u32 i = 0; i < 12; ++i) {
                glDrawArrays(GL_TRIANGLE_STRIP, 4 * i, 4);
            }
        }

        if (ANTON_UNLIKELY(vao == 0)) {
            initialize_ogl();
        }

        Shader& uniform_color_shader = get_builtin_shader(Builtin_Shader::uniform_color_3d);
        uniform_color_shader.use();
        uniform_color_shader.set_vec4("color", arrow.color);
        float scale = compute_scale(world_transform, arrow.size, view_projection_matrix, viewport_size);
        uniform_color_shader.set_matrix4("mvp_mat", math::transform::scale(scale) * world_transform * view_projection_matrix);
        glBindVertexArray(vao);
        switch (arrow.draw_style) {
            case Arrow_3D_Style::cone: {
                glDrawArrays(GL_LINES, cone_base_index, 2);
                GLint const first[2] = {cone_base_index + 2, cone_base_index + 2 + cone_base_point_count + 2};
                GLsizei const count[2] = {cone_base_point_count + 2, cone_base_point_count + 2};
                glMultiDrawArrays(GL_TRIANGLE_FAN, first, count, 2);
            } break;
            case Arrow_3D_Style::cube: {
                glDrawArrays(GL_LINES, cube_base_index, 2);
                glDrawArrays(GL_TRIANGLE_STRIP, cube_base_index + 2, cube_vertex_count);
            } break;
        }
    }

    anton_stl::Optional<float> intersect_arrow_3d(Ray const ray, Arrow_3D const arrow, Matrix4 const world_transform, Matrix4 const view_projection_matrix,
                                                  Vector2 const viewport_size) {
        float const scale = compute_scale(world_transform, arrow.size, view_projection_matrix, viewport_size);
        switch (arrow.draw_style) {
            case Arrow_3D_Style::cone: {
                OBB line_bounding_vol;
                line_bounding_vol.local_x = Vector3(Vector4(Vector3::right, 0.0f) * world_transform);
                line_bounding_vol.local_y = Vector3(Vector4(Vector3::up, 0.0f) * world_transform);
                line_bounding_vol.local_z = Vector3(Vector4(Vector3::forward, 0.0f) * world_transform);
                line_bounding_vol.halfwidths = {0.05f * scale, 0.05f * scale, 0.4f * scale};
                line_bounding_vol.center = math::transform::get_translation(world_transform) + line_bounding_vol.local_z * 0.4 * scale;
                if (anton_stl::Optional<Raycast_Hit> const hit = intersect_ray_obb(ray, line_bounding_vol); hit.holds_value()) {
                    return hit->distance;
                } else {
                    return anton_stl::null_optional;
                }
            }
            case Arrow_3D_Style::cube: {
                OBB line_bounding_vol;
                line_bounding_vol.local_x = Vector3(Vector4(Vector3::right, 0.0f) * world_transform);
                line_bounding_vol.local_y = Vector3(Vector4(Vector3::up, 0.0f) * world_transform);
                line_bounding_vol.local_z = Vector3(Vector4(Vector3::forward, 0.0f) * world_transform);
                line_bounding_vol.halfwidths = {0.05f * scale, 0.05f * scale, 0.4f * scale};
                line_bounding_vol.center = math::transform::get_translation(world_transform) + line_bounding_vol.local_z * 0.4;
                if (anton_stl::Optional<Raycast_Hit> const hit = intersect_ray_obb(ray, line_bounding_vol); hit.holds_value()) {
                    return hit->distance;
                } else {
                    return anton_stl::null_optional;
                }
            }
        }
    }

    void draw_dial_3d(Dial_3D const dial, Matrix4 const world_transform, Matrix4 const view_mat, Matrix4 const projection_mat, Vector3 const camera_pos,
                      Vector2 const viewport_size) {
        if (ANTON_UNLIKELY(vao == 0)) {
            initialize_ogl();
        }

        Shader& uniform_color_shader = get_builtin_shader(Builtin_Shader::uniform_color_line_3d);
        uniform_color_shader.use();
        Matrix4 const vp_mat = view_mat * projection_mat;
        float scale = compute_scale(world_transform, dial.size, vp_mat, viewport_size);
        uniform_color_shader.set_matrix4("model_mat", math::transform::scale(scale) * world_transform);
        uniform_color_shader.set_matrix4("vp_mat", vp_mat);
        uniform_color_shader.set_vec4("color", dial.color);
        uniform_color_shader.set_vec3("camera_pos", camera_pos);
        uniform_color_shader.set_float("line_width", scale * 0.018f);
        glBindVertexArray(line_vao);
        // TODO: Temporarily rebind on each draw because debug geometry binds its own offsets.
        u64 const base_offset = offsetof(Gizmos_Vertex_Data, dial);
        glBindVertexBuffer(0, vbo, base_offset + offsetof(Dial_Vertex_Data, vertices), sizeof(Vector3));
        glBindVertexBuffer(1, vbo, base_offset + offsetof(Dial_Vertex_Data, normals), sizeof(Vector3));
        glBindVertexBuffer(2, vbo, base_offset + offsetof(Dial_Vertex_Data, scale_factors), sizeof(float));
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 2 * dial_vertex_count + 2);
    }
} // namespace anton_engine::gizmo
