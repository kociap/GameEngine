#include <math/transform.hpp>

#include <cmath>

namespace math {
    namespace transform {
        Matrix4 translate(Vector3 translation) {
            return {{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {translation.x, translation.y, translation.z, 1}};
        }

        Matrix4 rotate(Quaternion q) {
            return {{1 - 2 * q.y * q.y - 2 * q.z * q.z, 2 * q.x * q.y + 2 * q.z * q.w, 2 * q.x * q.z - 2 * q.y * q.w, 0},
                    {2 * q.x * q.y - 2 * q.z * q.w, 1 - 2 * q.x * q.x - 2 * q.z * q.z, 2 * q.y * q.z + 2 * q.x * q.w, 0},
                    {2 * q.x * q.z + 2 * q.y * q.w, 2 * q.y * q.z - 2 * q.x * q.w, 1 - 2 * q.x * q.x - 2 * q.y * q.y, 0},
                    {0, 0, 0, 1}};
        }

        Matrix4 rotate_x(float angle) {
            float sin_val = sinf(angle);
            float cos_val = cosf(angle);
            return {{1, 0, 0, 0}, {0, cos_val, -sin_val, 0}, {0, sin_val, cos_val, 0}, {0, 0, 0, 1}};
        }

        Matrix4 rotate_y(float angle) {
            float sin_val = sinf(angle);
            float cos_val = cosf(angle);
            return {{cos_val, 0, sin_val, 0}, {0, 1, 0, 0}, {-sin_val, 0, cos_val, 0}, {0, 0, 0, 1}};
        }

        Matrix4 rotate_z(float angle) {
            float sin_val = sinf(angle);
            float cos_val = cosf(angle);
            return {{cos_val, -sin_val, 0, 0}, {sin_val, cos_val, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}};
        }

        Matrix4 scale(Vector3 scale) {
            return {{scale.x, 0, 0, 0}, {0, scale.y, 0, 0}, {0, 0, scale.z, 0}, {0, 0, 0, 1}};
        }

        Matrix4 scale(float scale) {
            return {{scale, 0, 0, 0}, {0, scale, 0, 0}, {0, 0, scale, 0}, {0, 0, 0, 1}};
        }

        Matrix4 orthographic(float left, float right, float bottom, float top, float near, float far) {
#ifdef TRANSFORM_COORDINATE_LEFT_HANDED
            return {{2.0f / (right - left), 0, 0, 0},
                    {0, 2.0f / (top - bottom), 0, 0},
                    {0, 0, 2.0f / (far - near), 0},
                    {-(right + left) / (right - left), -(top + bottom) / (top - bottom), -(far + near) / (far - near), 1}};
#else
            return {{2.0f / (right - left), 0, 0, 0},
                    {0, 2.0f / (top - bottom), 0, 0},
                    {0, 0, -2.0f / (far - near), 0},
                    {-(right + left) / (right - left), -(top + bottom) / (top - bottom), -(far + near) / (far - near), 1}};
#endif
        }

        Matrix4 perspective(float fov, float aspect_ratio, float near, float far) {
            float inv_tan = 1 / (tanf(fov / 2));
#ifdef TRANSFORM_COORDINATE_LEFT_HANDED
            return {{inv_tan / aspect_ratio, 0, 0, 0}, {0, inv_tan, 0, 0}, {0, 0, (far + near) / (far - near), 1}, {0, 0, -2 * near * far / (far - near), 0}};
#else
            return {{inv_tan / aspect_ratio, 0, 0, 0}, {0, inv_tan, 0, 0}, {0, 0, -(far + near) / (far - near), -1}, {0, 0, -2 * far * near / (far - near), 0}};
#endif
        }

        Matrix4 look_at(Vector3 position, Vector3 target, Vector3 up_vec) {
            Vector3 forward = (target - position).normalize();
            Vector3 right = math::cross(forward, up_vec).normalize();
            Vector3 up = math::cross(right, forward);
            return {{right.x, up.x, -forward.x, 0},
                    {right.y, up.y, -forward.y, 0},
                    {right.z, up.z, -forward.z, 0},
                    {-math::dot(right, position), -math::dot(up, position), math::dot(forward, position), 1}};
        }

        Vector3 get_translation(Matrix4 const& mat) {
            return {mat(3, 0), mat(3, 1), mat(3, 2)};
        }
    } // namespace transform
} // namespace math
