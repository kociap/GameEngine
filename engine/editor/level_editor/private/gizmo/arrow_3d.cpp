#include <gizmo/arrow_3d.hpp>

#include <anton_stl/vector.hpp>
#include <builtin_shaders.hpp>
#include <gizmo/gizmo.hpp>
#include <glad.hpp>
#include <intrinsics.hpp>
#include <math/quaternion.hpp>
#include <math/transform.hpp>
#include <math/vector3.hpp>

#include <debug_macros.hpp> // CHECK_GL_ERRORS

namespace anton_engine::gizmo {
    constexpr uint32_t cone_base_point_count = 16;
    constexpr uint32_t cone_vertex_count = 2 * (cone_base_point_count + 2); // 2 * (circle with 1 duplicate + tip)

    constexpr uint32_t cube_vertex_count = 14;

    struct Arrow_Vertex_Data {
        Vector3 cone[2 + cone_vertex_count];
        Vector3 cube[2 + cube_vertex_count];
    };

    constexpr uint32_t cone_base_index = 0;
    constexpr uint32_t cube_base_index = 2 + cone_vertex_count;

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
        for (uint32_t i = 0; i <= cone_base_point_count; ++i) {
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

    void draw_arrow_3d(Arrow_3D const arrow, Matrix4 const world_transform, Matrix4 const view_projection_matrix, Vector2 const viewport_size) {
        // TODO: Local buffers to make this work
        static uint32_t vbo = 0;
        static uint32_t vao = 0;

        if (ANTON_UNLIKELY(vao == 0)) {
            glGenVertexArrays(1, &vao);
            glBindVertexArray(vao);
            glEnableVertexAttribArray(0);
            glVertexAttribFormat(0, 3, GL_FLOAT, GL_FALSE, 0);
            glVertexAttribBinding(0, 0);
            CHECK_GL_ERRORS();

            Arrow_Vertex_Data arrow_vertex_data;
            generate_cone_geometry(arrow_vertex_data.cone);
            generate_cube_geometry(arrow_vertex_data.cube);

            glGenBuffers(1, &vbo);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferStorage(GL_ARRAY_BUFFER, sizeof(Arrow_Vertex_Data), &arrow_vertex_data, 0);
            glBindVertexBuffer(0, vbo, 0, sizeof(Vector3));
            CHECK_GL_ERRORS();
        } else {
            glBindVertexArray(vao);
        }

        Shader& uniform_color_shader = get_builtin_shader(Builtin_Shader::uniform_color_3d);
        uniform_color_shader.use();
        uniform_color_shader.set_vec4("color", arrow.color);
        float scale = compute_scale(world_transform, arrow.size, view_projection_matrix, viewport_size);
        uniform_color_shader.set_matrix4("mvp_mat", math::transform::scale(scale) * world_transform * view_projection_matrix);
        glDisable(GL_DEPTH_TEST);
        CHECK_GL_ERRORS();
        if (arrow.draw_style == Arrow_3D_Style::cone) {
            glDrawArrays(GL_LINES, cone_base_index, 2);
            GLint first[2] = {cone_base_index + 2, cone_base_index + 2 + cone_base_point_count + 2};
            GLsizei count[2] = {cone_base_point_count + 2, cone_base_point_count + 2};
            glMultiDrawArrays(GL_TRIANGLE_FAN, first, count, 2);
            CHECK_GL_ERRORS();
        } else {
            glDrawArrays(GL_LINES, cube_base_index, 2);
            glDrawArrays(GL_TRIANGLE_STRIP, cube_base_index + 2, cube_vertex_count);
            CHECK_GL_ERRORS();
        }
        glEnable(GL_DEPTH_TEST);
    }
} // namespace anton_engine::gizmo
