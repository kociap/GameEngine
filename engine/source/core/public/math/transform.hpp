#ifndef GAME_ENGINE_CORE_MATH_TRANSFORM_HPP_INCLUDE
#define GAME_ENGINE_CORE_MATH_TRANSFORM_HPP_INCLUDE

#include "matrix4.hpp"
#include "vector3.hpp"
#include "quaternion.hpp"

namespace transform {
    Matrix4 translate(Vector3 const& translation);
    Matrix4 rotate(Vector3 const& axis, float angle);
    Matrix4 rotate(Quaternion const& quaternion);
    Matrix4 rotate_x(float angle);
    Matrix4 rotate_y(float angle);
    Matrix4 rotate_z(float angle);
    Matrix4 scale(Vector3 const& scale);
    Matrix4 orthographic(float left, float right, float bottom, float top, float near, float far);
    Matrix4 perspective(float fov, float aspect_ratio, float near, float far);
    Matrix4 look_at(Vector3 position, Vector3 target, Vector3 up);

	Vector3 get_translation(Matrix4 const&);
} // namespace transform

#endif // !GAME_ENGINE_CORE_MATH_TRANSFORM_HPP_INCLUDE
