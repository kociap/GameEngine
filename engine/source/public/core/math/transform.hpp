#ifndef CORE_MATH_TRANSFORM_HPP_INCLUDE
#define CORE_MATH_TRANSFORM_HPP_INCLUDE

#include <core/macro_undefs.hpp>
#include <core/math/matrix4.hpp>
#include <core/math/quaternion.hpp>
#include <core/math/vector3.hpp>
#include <core/math/math.hpp>

namespace anton_engine::math::transform {
    Matrix4 translate(Vector3 translation);
    Matrix4 rotate(Vector3 axis, f32 angle);
    Matrix4 rotate(Quaternion quaternion);
    Matrix4 rotate_x(f32 angle);
    Matrix4 rotate_y(f32 angle);
    Matrix4 rotate_z(f32 angle);
    Matrix4 scale(Vector3 scale);
    Matrix4 scale(f32 scale);
    Matrix4 orthographic(f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far);
    Matrix4 perspective(f32 fov, f32 aspect_ratio, f32 near, f32 far);

    Vector3 get_translation(Matrix4);
} // namespace anton_engine::math::transform

namespace anton_engine::math::transform {
    inline Matrix4 translate(Vector3 translation) {
        return {{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {translation.x, translation.y, translation.z, 1}};
    }

    inline Matrix4 rotate(Quaternion q) {
        return {{1 - 2 * q.y * q.y - 2 * q.z * q.z, 2 * q.x * q.y + 2 * q.z * q.w, 2 * q.x * q.z - 2 * q.y * q.w, 0},
                {2 * q.x * q.y - 2 * q.z * q.w, 1 - 2 * q.x * q.x - 2 * q.z * q.z, 2 * q.y * q.z + 2 * q.x * q.w, 0},
                {2 * q.x * q.z + 2 * q.y * q.w, 2 * q.y * q.z - 2 * q.x * q.w, 1 - 2 * q.x * q.x - 2 * q.y * q.y, 0},
                {0, 0, 0, 1}};
    }

    inline Matrix4 rotate_x(f32 angle) {
        f32 sin_val = sinf(angle);
        f32 cos_val = cosf(angle);
        return {{1, 0, 0, 0}, {0, cos_val, -sin_val, 0}, {0, sin_val, cos_val, 0}, {0, 0, 0, 1}};
    }

    inline Matrix4 rotate_y(f32 angle) {
        f32 sin_val = sinf(angle);
        f32 cos_val = cosf(angle);
        return {{cos_val, 0, sin_val, 0}, {0, 1, 0, 0}, {-sin_val, 0, cos_val, 0}, {0, 0, 0, 1}};
    }

    inline Matrix4 rotate_z(f32 angle) {
        f32 sin_val = sinf(angle);
        f32 cos_val = cosf(angle);
        return {{cos_val, -sin_val, 0, 0}, {sin_val, cos_val, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}};
    }

    inline Matrix4 scale(Vector3 scale) {
        return {{scale.x, 0, 0, 0}, {0, scale.y, 0, 0}, {0, 0, scale.z, 0}, {0, 0, 0, 1}};
    }

    inline Matrix4 scale(f32 scale) {
        return {{scale, 0, 0, 0}, {0, scale, 0, 0}, {0, 0, scale, 0}, {0, 0, 0, 1}};
    }

    inline Matrix4 orthographic(f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far) {
        return {{2.0f / (right - left), 0, 0, 0},
                {0, 2.0f / (top - bottom), 0, 0},
                {0, 0, -2.0f / (far - near), 0},
                {-(right + left) / (right - left), -(top + bottom) / (top - bottom), -(far + near) / (far - near), 1}};

        // left handed
        // return {{2.0f / (right - left), 0, 0, 0},
        //         {0, 2.0f / (top - bottom), 0, 0},
        //         {0, 0, 2.0f / (far - near), 0},
        //         {-(right + left) / (right - left), -(top + bottom) / (top - bottom), -(far + near) / (far - near), 1}};
    }

    inline Matrix4 perspective(f32 fov, f32 aspect_ratio, f32 near, f32 far) {
        f32 inv_tan = 1 / (math::tan(fov / 2));
        return {{inv_tan / aspect_ratio, 0, 0, 0}, {0, inv_tan, 0, 0}, {0, 0, -(far + near) / (far - near), -1}, {0, 0, -2 * far * near / (far - near), 0}};

        // left handed
        // return {{inv_tan / aspect_ratio, 0, 0, 0}, {0, inv_tan, 0, 0}, {0, 0, (far + near) / (far - near), 1}, {0, 0, -2 * near * far / (far - near), 0}};
    }

    inline Vector3 get_translation(Matrix4 mat) {
        return {mat[3][0], mat[3][1], mat[3][2]};
    }
} // namespace anton_engine::math::transform

#endif // !CORE_MATH_TRANSFORM_HPP_INCLUDE
