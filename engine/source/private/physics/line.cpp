#include <physics/line.hpp>

#include <core/math/math.hpp>
#include <core/math/matrix2.hpp>

namespace anton_engine {
    Vector2 closest_points_on_lines(Line l1, Line l2) {
        Matrix2 inv = math::inverse(Matrix2({math::dot(l1.direction, l1.direction), math::dot(l1.direction, l2.direction)},
                                            {-math::dot(l1.direction, l2.direction), -math::dot(l2.direction, l2.direction)}));
        Vector2 result = Vector2(math::dot(l2.origin - l1.origin, l1.direction), math::dot(l2.origin - l1.origin, l2.direction)) * inv;
        return result;
    }
} // namespace anton_engine
