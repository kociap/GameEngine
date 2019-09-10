#ifndef ENGINE_COMPONENTS_TRANSFORM_COMPONENT_HPP_INCLUDE
#define ENGINE_COMPONENTS_TRANSFORM_COMPONENT_HPP_INCLUDE

#include <class_macros.hpp>
#include <math/matrix4.hpp>
#include <math/quaternion.hpp>
#include <math/vector3.hpp>
#include <serialization/serialization.hpp>

namespace anton_engine {
    class COMPONENT Transform {
    public:
        Quaternion local_rotation;
        Vector3 local_position;
        Vector3 local_scale = Vector3::one;

        void translate(Vector3 const&);
        void scale(Vector3 const&);
        // axis to rotate about
        // angle in radians
        void rotate(Vector3 axis, float angle);
        Matrix4 to_matrix() const;
    };
} // namespace anton_engine

ANTON_DEFAULT_SERIALIZABLE(anton_engine::Transform);
ANTON_DEFAULT_DESERIALIZABLE(anton_engine::Transform);

#endif // !ENGINE_COMPONENTS_TRANSFORM_COMPONENT_HPP_INCLUDE
