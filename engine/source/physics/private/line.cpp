#include "line.hpp"

#include "math/math.hpp"
#include "math/matrix2.hpp"

Vector2 closest_points_on_lines(Line l1, Line l2) {
    Matrix2 inv = math::inverse(Matrix2({math::dot(l1.direction, l1.direction), math::dot(l1.direction, l2.direction)},
                                        {-math::dot(l1.direction, l2.direction), -math::dot(l2.direction, l2.direction)}));
    Vector2 result = Vector2(math::dot(l2.origin - l1.origin, l1.direction), math::dot(l2.origin - l1.origin, l2.direction)) * inv;
    return result;
}

bool intersect_line_plane(Line line, Vector3 normal, float distance, Linecast_Hit& out) {
    float angle_cos = math::dot(line.direction, normal);
    float coeff = (distance - math::dot(line.origin, normal)) / angle_cos;
    out.distance = coeff;
    out.hit_point = line.origin + line.direction * coeff;
    return math::abs(angle_cos) < math::constants<float>::epsilon;
}