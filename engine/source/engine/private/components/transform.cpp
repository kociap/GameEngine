#include "components/transform.hpp"
#include "debug_macros.hpp"

#include <cmath>

void Transform::rotate(Vector3 axis, float angle) {
    float half_angle_sin = std::sinf(angle / 2);
    rotation = Quaternion(axis.x * half_angle_sin, axis.y * half_angle_sin, axis.z * half_angle_sin, std::cosf(angle / 2)) * rotation;
}

Matrix4 Transform::to_matrix() const {
    GE_assert(false, "Transform::to_matrix is ot implemented");
    // TODO implement
    return Matrix4::identity;
}