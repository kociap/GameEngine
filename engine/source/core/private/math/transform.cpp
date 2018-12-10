#include "math/transform.hpp"

namespace transform {
    Matrix4 translate(Matrix4 const& mat, Vector3 const& translation) {
        Matrix4 translation_mat({1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {translation.x, translation.y, translation.z, 1});
        return mat * translation_mat;
    }

    Matrix4 rotate_x(Matrix4 const& mat, float angle) {
        float sin_val = sin(angle);
        float cos_val = cos(angle);
        Matrix4 rotation_mat({1, 0, 0, 0}, {0, cos_val, -sin_val, 0}, {0, sin_val, cos_val, 0}, {0, 0, 0, 1});
        return mat * rotation_mat;
    }

    Matrix4 rotate_y(Matrix4 const& mat, float angle) {
        float sin_val = sin(angle);
        float cos_val = cos(angle);
        Matrix4 rotation_mat({cos_val, 0, sin_val, 0}, {0, 1, 0, 0}, {-sin_val, 0, cos_val, 0}, {0, 0, 0, 1});
        return mat * rotation_mat;
    }

    Matrix4 rotate_z(Matrix4 const& mat, float angle) {
        float sin_val = sin(angle);
        float cos_val = cos(angle);
        Matrix4 rotation_mat({cos_val, -sin_val, 0, 0}, {sin_val, cos_val, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1});
        return mat * rotation_mat;
    }

    Matrix4 scale(Matrix4 const& mat, Vector3 const& scale) {
        Matrix4 rotation_mat({scale.x, 0, 0, 0}, {0, scale.y, 0, 0}, {0, 0, scale.z, 0}, {0, 0, 0, 1});
        return mat * rotation_mat;
    }
} // namespace transform