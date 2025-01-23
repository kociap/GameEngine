#pragma once

#include <anton/math/mat4.hpp>
#include <anton/math/vec2.hpp>
#include <anton/optional.hpp>
#include <core/color.hpp>
#include <core/types.hpp>
#include <level_editor/gizmo/gizmo.hpp>
#include <physics/ray.hpp>

namespace anton_engine::gizmo {
  // Arrow head style
  enum class Arrow_3D_Style {
    // none
    cone,
    cube,
  };

  struct Arrow_3D {
    Arrow_3D_Style draw_style;
    Color color;
    // Size in pixels
    u32 size;
  };

  // Renders an arrow from Vec3::zero to Vec3::forward that is transformed by world_transform.
  void draw_arrow_3d(Arrow_3D, Mat4 world_transform,
                     Mat4 view_projection_matrix, Vec2 viewport_size,
                     Vec3 camera_pos);
  // Tests the arrow for intersection and returns the distance to the intersection point along the ray if the ray intersects the bounding volumes.
  anton::Optional<float> intersect_arrow_3d(Ray, Arrow_3D, Mat4 world_transform,
                                            Mat4 view_projection_matrix,
                                            Vec2 viewport_size);
} // namespace anton_engine::gizmo
