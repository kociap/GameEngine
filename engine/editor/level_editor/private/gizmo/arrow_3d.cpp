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
    void draw_arrow_3d(Arrow_3D const arrow, Matrix4 const world_transform, Matrix4 const view_projection_matrix, Vector2 const viewport_size) {
        // TODO: Local buffers to make this work
        static uint32_t vbo = 0;
        static uint32_t vao = 0;

        constexpr uint32_t base_point_count = 16;
        constexpr uint32_t vertex_count = 2 + 2 * (base_point_count + 2); // line + 2 * (circle with 1 duplicate + tip)

        if (ANTON_UNLIKELY(vao == 0)) {
            glGenVertexArrays(1, &vao);
            glBindVertexArray(vao);
            glEnableVertexAttribArray(0);
            glVertexAttribFormat(0, 3, GL_FLOAT, GL_FALSE, 0);
            glVertexAttribBinding(0, 0);
            CHECK_GL_ERRORS();

            Vector3 arrow_vertex_data[vertex_count];
            arrow_vertex_data[0] = {0, 0, 0};
            arrow_vertex_data[1] = {0, 0, -0.8f};

            float angle = math::radians(360.0f / static_cast<float>(base_point_count));
            Vector3 rotation_axis = Vector3::forward * math::sin(angle / 2.0f);
            Quaternion rotation_quat(rotation_axis.x, rotation_axis.y, rotation_axis.z, math::cos(angle / 2.0f));

            Vector3 vertex = {0, 0.05f, 0};
            Quaternion rotated_vec = Quaternion(vertex.x, vertex.y, vertex.z, 0);
            Vector3* const base = arrow_vertex_data + 2 + base_point_count + 1;
            Vector3* const cone = arrow_vertex_data + 2;
            base[0] = {0, 0, -0.8f};
            cone[0] = {0, 0, -1.0f};
            for (uint32_t i = 0; i <= base_point_count; ++i) {
                rotated_vec = rotation_quat * rotated_vec * math::conjugate(rotation_quat);
                base[1 + base_point_count - i] = Vector3{rotated_vec.x, rotated_vec.y, -0.8f};
                cone[1 + i] = Vector3{rotated_vec.x, rotated_vec.y, -0.8f};
            }

            glGenBuffers(1, &vbo);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferStorage(GL_ARRAY_BUFFER, vertex_count * sizeof(Vector3), arrow_vertex_data, 0);
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
        glDrawArrays(GL_LINES, 0, 2);
        CHECK_GL_ERRORS();
        // TODO: 1 missing triangle. base is rendered in clockwise winding instead of ccw.
        GLint first[2] = {2, 2 + base_point_count + 2};
        GLsizei count[2] = {base_point_count + 2, base_point_count + 2};
        glMultiDrawArrays(GL_TRIANGLE_FAN, first, count, 2);
        CHECK_GL_ERRORS();
        glEnable(GL_DEPTH_TEST);
        CHECK_GL_ERRORS();
    }
} // namespace anton_engine::gizmo
