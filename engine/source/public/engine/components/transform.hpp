#ifndef ENGINE_COMPONENTS_TRANSFORM_COMPONENT_HPP_INCLUDE
#define ENGINE_COMPONENTS_TRANSFORM_COMPONENT_HPP_INCLUDE

#include <core/class_macros.hpp>
#include <core/math/matrix4.hpp>
#include <core/math/quaternion.hpp>
#include <core/math/vector3.hpp>
#include <core/serialization/serialization.hpp>
#include <core/math/transform.hpp>

namespace anton_engine {
    class COMPONENT Transform {
    public:
        Quaternion local_rotation;
        Vector3 local_position;
        Vector3 local_scale = Vector3::one;

        void translate(Vector3 const& translation_vec) {
            local_position += translation_vec;
        }

        void scale(Vector3 const& scale_vec) {
            local_scale = math::multiply_componentwise(local_scale, scale_vec);
        }

        // axis to rotate about
        // angle in radians
        void rotate(Vector3 axis, float angle) {
            float half_angle_sin = std::sinf(angle / 2);
            Quaternion q(axis.x * half_angle_sin, axis.y * half_angle_sin, axis.z * half_angle_sin, std::cosf(angle / 2));
            local_rotation = q * local_rotation;
        }

        Matrix4 to_matrix() const {
            Matrix4 mat = math::transform::scale(local_scale) * math::transform::rotate(local_rotation) * math::transform::translate(local_position);
            return mat;
        }
    };

    inline Matrix4 to_matrix(Transform const t) {
        Matrix4 mat = math::transform::scale(t.local_scale) * math::transform::rotate(t.local_rotation) * math::transform::translate(t.local_position);
        return mat;
    }
} // namespace anton_engine

ANTON_DEFAULT_SERIALIZABLE(anton_engine::Transform);

#endif // !ENGINE_COMPONENTS_TRANSFORM_COMPONENT_HPP_INCLUDE
