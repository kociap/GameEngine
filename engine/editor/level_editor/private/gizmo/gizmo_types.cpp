#include <gizmo/arrow_3d.hpp>
#include <gizmo/dial_3d.hpp>

#include <anton_stl/vector.hpp>
#include <builtin_shaders.hpp>
#include <gizmo/gizmo.hpp>
#include <glad.hpp>
#include <intrinsics.hpp>
#include <math/math.hpp>
#include <math/quaternion.hpp>
#include <math/transform.hpp>
#include <math/vector3.hpp>

#include <debug_macros.hpp> // CHECK_GL_ERRORS

namespace anton_engine::gizmo {
    constexpr int32_t cone_base_point_count = 16;
    constexpr int32_t cone_vertex_count = 2 * (cone_base_point_count + 2); // 2 * (circle with 1 duplicate + tip)
    constexpr int32_t cube_vertex_count = 14;
    constexpr int32_t dial_vertex_count = 32;

    struct Arrow_Vertex_Data {
        Vector3 cone[2 + cone_vertex_count];
        Vector3 cube[2 + cube_vertex_count];
    };

    struct Dial_Vertex_Data {
        Vector3 vertices[2 * dial_vertex_count + 2];
        Vector3 normals[2 * dial_vertex_count + 2];
        float scale_factors[2 * dial_vertex_count + 2];
    };

    struct Gizmos_Vertex_Data {
        Arrow_Vertex_Data arrow;
        Dial_Vertex_Data dial;
    };

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
        for (int32_t i = 0; i <= cone_base_point_count; ++i) {
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
        for (int32_t i = 0; i <= 2 * dial_vertex_count; i += 2) {
            Vector3 const pos = {math::cos(angle * i), math::sin(angle * i), 0};
            dial->vertices[i] = pos;
            dial->vertices[i + 1] = pos;
        }

        for (int32_t i = 2; i <= 2 * dial_vertex_count - 2; i += 2) {
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
    static uint32_t vbo = 0;
    static uint32_t vao = 0;
    static uint32_t line_vao = 0;

    constexpr int32_t cone_base_index = 0;
    constexpr int32_t cube_base_index = 2 + cone_vertex_count;

    static void initialize_ogl() {
        Gizmos_Vertex_Data gizmos_vertex_data;
        generate_cone_geometry(gizmos_vertex_data.arrow.cone);
        generate_cube_geometry(gizmos_vertex_data.arrow.cube);
        generate_dial_geometry(&gizmos_vertex_data.dial);

        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferStorage(GL_ARRAY_BUFFER, sizeof(Gizmos_Vertex_Data), &gizmos_vertex_data, 0);

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
        uint64_t base_offset = offsetof(Gizmos_Vertex_Data, dial);
        glBindVertexBuffer(0, vbo, base_offset + offsetof(Dial_Vertex_Data, vertices), sizeof(Vector3));
        glBindVertexBuffer(1, vbo, base_offset + offsetof(Dial_Vertex_Data, normals), sizeof(Vector3));
        glBindVertexBuffer(2, vbo, base_offset + offsetof(Dial_Vertex_Data, scale_factors), sizeof(float));
    }

    void draw_arrow_3d(Arrow_3D const arrow, Matrix4 const world_transform, Matrix4 const view_projection_matrix, Vector2 const viewport_size) {
        if (ANTON_UNLIKELY(vao == 0)) {
            initialize_ogl();
        }

        Shader& uniform_color_shader = get_builtin_shader(Builtin_Shader::uniform_color_3d);
        uniform_color_shader.use();
        uniform_color_shader.set_vec4("color", arrow.color);
        float scale = compute_scale(world_transform, arrow.size, view_projection_matrix, viewport_size);
        uniform_color_shader.set_matrix4("mvp_mat", math::transform::scale(scale) * world_transform * view_projection_matrix);
        glBindVertexArray(vao);
        if (arrow.draw_style == Arrow_3D_Style::cone) {
            glDrawArrays(GL_LINES, cone_base_index, 2);
            GLint first[2] = {cone_base_index + 2, cone_base_index + 2 + cone_base_point_count + 2};
            GLsizei count[2] = {cone_base_point_count + 2, cone_base_point_count + 2};
            glMultiDrawArrays(GL_TRIANGLE_FAN, first, count, 2);
        } else {
            glDrawArrays(GL_LINES, cube_base_index, 2);
            glDrawArrays(GL_TRIANGLE_STRIP, cube_base_index + 2, cube_vertex_count);
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
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 2 * dial_vertex_count + 2);
    }
} // namespace anton_engine::gizmo
