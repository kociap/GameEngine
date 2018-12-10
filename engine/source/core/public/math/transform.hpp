#ifndef GAME_ENGINE_CORE_MATH_TRANSFORM_HPP_INCLUDE
#define GAME_ENGINE_CORE_MATH_TRANSFORM_HPP_INCLUDE

#include "matrix4.hpp"
#include "vector3.hpp"

namespace transform {
    Matrix4 translate(Matrix4 const& matrix, Vector3 const& translation);
    Matrix4 rotate(Matrix4 const& matrix, Vector3 const& axis, float angle);
    Matrix4 rotate_x(Matrix4 const& matrix, float angle);
    Matrix4 rotate_y(Matrix4 const& matrix, float angle);
    Matrix4 rotate_z(Matrix4 const& matrix, float angle);
    Matrix4 scale(Matrix4 const& matrix, Vector3 const& scale);
    Matrix4 perspective();
    Matrix4 look_at();
} // namespace transform

#endif // !GAME_ENGINE_CORE_MATH_TRANSFORM_HPP_INCLUDE
