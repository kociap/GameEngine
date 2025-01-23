#pragma once

#include <anton/math/vec3.hpp>
#include <core/types.hpp>

namespace anton_engine {
  class Raycast_Hit {
  public:
    Vec3 hit_point;
    Vec3 barycentric_coordinates;
    f32 distance = 0;
  };
} // namespace anton_engine
