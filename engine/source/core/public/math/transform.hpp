#ifndef GAME_ENGINE_CORE_MATH_TRANSFORM_HPP_INCLUDE
#define GAME_ENGINE_CORE_MATH_TRANSFORM_HPP_INCLUDE

#include "matrix4.hpp"
#include "vector3.hpp"

Matrix4 translate(Matrix4 matrix, Vector3 const& translation);
Matrix4 rotate(Matrix4 matrix, Vector3 const& axis, float angle);
Matrix4 scale(Matrix4 matrix, Vector3 const& scale);
Matrix4 perspective();
Matrix4 look_at();

#endif // !GAME_ENGINE_CORE_MATH_TRANSFORM_HPP_INCLUDE
