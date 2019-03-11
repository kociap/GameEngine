#include "components/transform.hpp"
#include "math/transform.hpp"

#include <cmath>

void Transform::translate(Vector3 const& translation_vec) {
    local_position += translation_vec;
}

void Transform::scale(Vector3 const& scale_vec) {
    local_scale.multiply_componentwise(scale_vec);
}

void Transform::rotate(Vector3 axis, float angle) {
    float half_angle_sin = std::sinf(angle / 2);
    Quaternion q(axis.x * half_angle_sin, axis.y * half_angle_sin, axis.z * half_angle_sin, std::cosf(angle / 2));
    local_rotation = q * local_rotation;
}

Matrix4 Transform::to_matrix() const {
    Matrix4 mat = transform::scale(local_scale) * transform::rotate(local_rotation) * transform::translate(local_position);
    return mat;
}