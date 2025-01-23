#pragma once

#include <anton/math/mat4.hpp>
#include <anton/math/vec2.hpp>
#include <anton/math/vec3.hpp>
#include <core/types.hpp>

namespace anton_engine {
  class Ray {
  public:
    Vec3 origin;
    Vec3 direction;
  };

  Ray screen_to_ray(Mat4 inv_view, Mat4 inv_projection, i32 screen_width,
                    i32 screen_height, Vec2 point);
  Ray screen_to_ray(Mat4 inv_view_projection, i32 screen_width,
                    i32 screen_height, Vec2 point);
} // namespace anton_engine
