#pragma once

#include <anton/math/mat4.hpp>
#include <anton/math/vec2.hpp>
#include <core/types.hpp>

#include <anton/math/vec3.hpp>
#include <core/color.hpp>

namespace anton_engine::gizmo {
  enum Draw_Options {
    draw_options_no_scale = 1 << 0,
  };

  [[nodiscard]] float compute_scale(Mat4 world_transform, u32 target_size,
                                    Mat4 pers_mat, Vec2 viewport_size);

  void debug_draw_line(Vec3 f, Vec3 l, float time);
  void debug_commit_draw_lines(Mat4 vp_mat, Vec3 camera_pos, float delta_time);
} // namespace anton_engine::gizmo
