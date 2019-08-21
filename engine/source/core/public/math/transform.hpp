#ifndef GAME_ENGINE_CORE_MATH_TRANSFORM_HPP_INCLUDE
#define GAME_ENGINE_CORE_MATH_TRANSFORM_HPP_INCLUDE

#include <macro_undefs.hpp>
#include <math/matrix4.hpp>
#include <math/quaternion.hpp>
#include <math/vector3.hpp>

namespace math {
    namespace transform {
        Matrix4 translate(Vector3 translation);
        Matrix4 rotate(Vector3 axis, float angle);
        Matrix4 rotate(Quaternion quaternion);
        Matrix4 rotate_x(float angle);
        Matrix4 rotate_y(float angle);
        Matrix4 rotate_z(float angle);
        Matrix4 scale(Vector3 scale);
        Matrix4 scale(float scale);
        Matrix4 orthographic(float left, float right, float bottom, float top, float near, float far);
        Matrix4 perspective(float fov, float aspect_ratio, float near, float far);
        Matrix4 look_at(Vector3 position, Vector3 target, Vector3 up);

        Vector3 get_translation(Matrix4 const&);
    } // namespace transform
} // namespace math

#endif // !GAME_ENGINE_CORE_MATH_TRANSFORM_HPP_INCLUDE
