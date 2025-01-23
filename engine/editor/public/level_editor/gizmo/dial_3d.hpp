#pragma once

#include <anton/math/mat4.hpp>
#include <anton/math/vec2.hpp>
#include <anton/math/vec3.hpp>
#include <core/color.hpp>

namespace anton_engine::gizmo {
  struct Dial_3D {
    Color color;
    // size in pixels
    u32 size;
  };

  void draw_dial_3d(Dial_3D, Mat4 world_transform, Mat4 view_projection,
                    Vec3 camera_pos, Vec2 viewport_size);
  // Tests the arrow for intersection and returns the distance to the intersection point along the ray if the ray intersects the bounding volumes.
  anton::Optional<float> intersect_dial_3d(Ray, Dial_3D, Mat4 world_transform,
                                           Mat4 view_projection_matrix,
                                           Vec2 viewport_size);
} // namespace anton_engine::gizmo
