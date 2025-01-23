#pragma once

#include <anton/math/vec2.hpp>
#include <anton/math/vec3.hpp>
#include <core/types.hpp>

namespace anton_engine {
  class Line {
  public:
    Vec3 origin;
    Vec3 direction;
  };

  // x is coefficient of the first line, y of the second one
  Vec2 closest_points_on_lines(Line, Line);
} // namespace anton_engine
