#include "math/transform.hpp"

namespace transform {
    Matrix4 translate(Matrix4 const& mat, Vector3 const& translation) {
        Matrix4 translation_mat({1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {translation.x, translation.y, translation.z, 1});
        return mat * translation_mat;
    }

    Matrix4 translate(Vector3 const& translation) {
        return Matrix4({1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {translation.x, translation.y, translation.z, 1});
    }

    Matrix4 rotate_x(Matrix4 const& mat, float angle) {
        float sin_val = sinf(angle);
        float cos_val = cosf(angle);
        Matrix4 rotation_mat({1, 0, 0, 0}, {0, cos_val, -sin_val, 0}, {0, sin_val, cos_val, 0}, {0, 0, 0, 1});
        return mat * rotation_mat;
    }

    Matrix4 rotate_y(Matrix4 const& mat, float angle) {
        float sin_val = sinf(angle);
        float cos_val = cosf(angle);
        Matrix4 rotation_mat({cos_val, 0, sin_val, 0}, {0, 1, 0, 0}, {-sin_val, 0, cos_val, 0}, {0, 0, 0, 1});
        return mat * rotation_mat;
    }

    Matrix4 rotate_z(Matrix4 const& mat, float angle) {
        float sin_val = sinf(angle);
        float cos_val = cosf(angle);
        Matrix4 rotation_mat({cos_val, -sin_val, 0, 0}, {sin_val, cos_val, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1});
        return mat * rotation_mat;
    }

    Matrix4 rotate_x(float angle) {
        float sin_val = sinf(angle);
        float cos_val = cosf(angle);
        return Matrix4({1, 0, 0, 0}, {0, cos_val, -sin_val, 0}, {0, sin_val, cos_val, 0}, {0, 0, 0, 1});
    }

    Matrix4 rotate_y(float angle) {
        float sin_val = sinf(angle);
        float cos_val = cosf(angle);
        return Matrix4({cos_val, 0, sin_val, 0}, {0, 1, 0, 0}, {-sin_val, 0, cos_val, 0}, {0, 0, 0, 1});
    }

    Matrix4 rotate_z(float angle) {
        float sin_val = sinf(angle);
        float cos_val = cosf(angle);
        return Matrix4({cos_val, -sin_val, 0, 0}, {sin_val, cos_val, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1});
    }

    Matrix4 scale(Matrix4 const& mat, Vector3 const& scale) {
        Matrix4 rotation_mat({scale.x, 0, 0, 0}, {0, scale.y, 0, 0}, {0, 0, scale.z, 0}, {0, 0, 0, 1});
        return mat * rotation_mat;
    }

    Matrix4 scale(Vector3 const& scale) {
        return Matrix4({scale.x, 0, 0, 0}, {0, scale.y, 0, 0}, {0, 0, scale.z, 0}, {0, 0, 0, 1});
    }

    Matrix4 orthographic(float left, float right, float bottom, float top, float near, float far) {
        return Matrix4({2.0f / (right - left), 0, 0, 0}, {0, 2.0f / (top - bottom), 0, 0}, {0, 0, 2.0f / (far - near), 0},
                       {-(right + left) / (right - left), -(top + bottom) / (top - bottom), -(far + near) / (far - near), 1});
    }

    Matrix4 perspective(float fov, float aspect_ratio, float near, float far) {
        float inv_tan = 1 / (tanf(fov / 2));
        return Matrix4({inv_tan / aspect_ratio, 0, 0, 0}, {0, inv_tan, 0, 0}, {0, 0, (far + near) / (far - near), 1},
                       {0, 0, -2 * near * far / (far - near), 0});
    }
} // namespace transform