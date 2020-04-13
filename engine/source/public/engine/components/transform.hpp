#ifndef ENGINE_COMPONENTS_TRANSFORM_COMPONENT_HPP_INCLUDE
#define ENGINE_COMPONENTS_TRANSFORM_COMPONENT_HPP_INCLUDE

#include <core/class_macros.hpp>
#include <core/math/matrix4.hpp>
#include <core/math/quaternion.hpp>
#include <core/math/transform.hpp>
#include <core/math/vector3.hpp>
#include <core/serialization/serialization.hpp>

namespace anton_engine {
    class COMPONENT Transform {
    public:
        Quaternion local_rotation;
        Vector3 local_position;
        Vector3 local_scale = Vector3{1.0f, 1.0f, 1.0f};

        void translate(Vector3 const& translation_vec) {
            local_position += translation_vec;
        }

        void scale(Vector3 const& scale_vec) {
            local_scale *= scale_vec;
        }

        // axis to rotate about
        // angle in radians
        void rotate(Vector3 axis, float angle) {
            float half_angle_sin = math::sin(angle / 2);
            Quaternion q(axis.x * half_angle_sin, axis.y * half_angle_sin, axis.z * half_angle_sin, math::cos(angle / 2));
            local_rotation = q * local_rotation;
        }

        Matrix4 to_matrix() const {
            Matrix4 mat = math::scale(local_scale) * math::rotate(local_rotation) * math::translate(local_position);
            return mat;
        }
    };

    inline Matrix4 to_matrix(Transform const t) {
        Matrix4 mat = math::scale(t.local_scale) * math::rotate(t.local_rotation) * math::translate(t.local_position);
        return mat;
    }
} // namespace anton_engine

ANTON_DEFAULT_SERIALIZABLE(anton_engine::Transform)

#endif // !ENGINE_COMPONENTS_TRANSFORM_COMPONENT_HPP_INCLUDE
