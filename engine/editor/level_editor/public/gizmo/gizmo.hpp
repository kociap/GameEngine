#ifndef EDITOR_LEVEL_EDITOR_GIZMO_GIZMO_HPP_INCLUDE
#define EDITOR_LEVEL_EDITOR_GIZMO_GIZMO_HPP_INCLUDE

#include <anton_int.hpp>
#include <math/matrix4.hpp>
#include <math/vector2.hpp>

#include <color.hpp>
#include <math/vector3.hpp>

namespace anton_engine::gizmo {
    enum Draw_Options {
        draw_options_no_scale = 1 << 0,
    };

    [[nodiscard]] float compute_scale(Matrix4 world_transform, u32 target_size, Matrix4 pers_mat, Vector2 viewport_size);

    void debug_draw_line(Vector3 f, Vector3 l, float time);
    void debug_commit_draw_lines(Matrix4 vp_mat, Vector3 camera_pos, float delta_time);
} // namespace anton_engine::gizmo

#endif // !EDITOR_LEVEL_EDITOR_GIZMO_GIZMO_HPP_INCLUDE
