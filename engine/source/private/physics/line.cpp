#include <physics/line.hpp>

#include <anton/math/mat2.hpp>
#include <anton/math/math.hpp>

namespace anton_engine {
  Vec2 closest_points_on_lines(Line l1, Line l2)
  {
    Mat2 inv = math::inverse(Mat2({math::dot(l1.direction, l1.direction),
                                   math::dot(l1.direction, l2.direction)},
                                  {-math::dot(l1.direction, l2.direction),
                                   -math::dot(l2.direction, l2.direction)}));
    Vec2 result = inv * Vec2(math::dot(l2.origin - l1.origin, l1.direction),
                             math::dot(l2.origin - l1.origin, l2.direction));
    return result;
  }
} // namespace anton_engine
