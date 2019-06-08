#ifndef ENGINE_COMPONENTS_TRANSFORM_COMPONENT_HPP_INCLUDE
#define ENGINE_COMPONENTS_TRANSFORM_COMPONENT_HPP_INCLUDE

#include "math/matrix4.hpp"
#include "math/quaternion.hpp"
#include "math/vector3.hpp"
#include "serialization.hpp"

class Transform {
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

template <>
struct serialization::use_default_deserialize<Transform>: std::true_type {};

#endif // !ENGINE_COMPONENTS_TRANSFORM_COMPONENT_HPP_INCLUDE