#ifndef CORE_MATH_TRANSFORM_HPP_INCLUDE
#define CORE_MATH_TRANSFORM_HPP_INCLUDE

#include <macro_undefs.hpp>
#include <math/matrix4.hpp>
#include <math/quaternion.hpp>
#include <math/vector3.hpp>

namespace anton_engine::math::transform {
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

    inline Matrix4 rotate_x(float angle) {
        float sin_val = sinf(angle);
        float cos_val = cosf(angle);
        return {{1, 0, 0, 0}, {0, cos_val, -sin_val, 0}, {0, sin_val, cos_val, 0}, {0, 0, 0, 1}};
    }

    inline Matrix4 rotate_y(float angle) {
        float sin_val = sinf(angle);
        float cos_val = cosf(angle);
        return {{cos_val, 0, sin_val, 0}, {0, 1, 0, 0}, {-sin_val, 0, cos_val, 0}, {0, 0, 0, 1}};
    }

    inline Matrix4 rotate_z(float angle) {
        float sin_val = sinf(angle);
        float cos_val = cosf(angle);
        return {{cos_val, -sin_val, 0, 0}, {sin_val, cos_val, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}};
    }

    inline Matrix4 scale(Vector3 scale) {
        return {{scale.x, 0, 0, 0}, {0, scale.y, 0, 0}, {0, 0, scale.z, 0}, {0, 0, 0, 1}};
    }

    inline Matrix4 scale(float scale) {
        return {{scale, 0, 0, 0}, {0, scale, 0, 0}, {0, 0, scale, 0}, {0, 0, 0, 1}};
    }

    inline Matrix4 orthographic(float left, float right, float bottom, float top, float near, float far) {
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

    inline Matrix4 perspective(float fov, float aspect_ratio, float near, float far) {
        float inv_tan = 1 / (tanf(fov / 2));
        return {{inv_tan / aspect_ratio, 0, 0, 0}, {0, inv_tan, 0, 0}, {0, 0, -(far + near) / (far - near), -1}, {0, 0, -2 * far * near / (far - near), 0}};

        // left handed
        // return {{inv_tan / aspect_ratio, 0, 0, 0}, {0, inv_tan, 0, 0}, {0, 0, (far + near) / (far - near), 1}, {0, 0, -2 * near * far / (far - near), 0}};
    }

    inline Vector3 get_translation(Matrix4 mat) {
        return {mat[3][0], mat[3][1], mat[3][2]};
    }
} // namespace anton_engine::math::transform

#endif // !CORE_MATH_TRANSFORM_HPP_INCLUDE
