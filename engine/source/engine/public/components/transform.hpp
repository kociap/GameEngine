#ifndef ENGINE_COMPONENTS_TRANSFORM_COMPONENT_HPP_INCLUDE
#define ENGINE_COMPONENTS_TRANSFORM_COMPONENT_HPP_INCLUDE

#include "base_component.hpp"
#include "math/matrix4.hpp"
#include "math/vector3.hpp"
#include "math/quaternion.hpp"

class Transform : public Base_Component {
public:
    Quaternion local_rotation;
    Vector3 local_position;
    Vector3 local_scale = Vector3::one;

public:
    using Base_Component::Base_Component;

    void translate(Vector3 const&);
    void scale(Vector3 const&);
	// axis to rotate about
	// angle in radians
    void rotate(Vector3 axis, float angle);
    Matrix4 to_matrix() const;
};

#endif // !ENGINE_COMPONENTS_TRANSFORM_COMPONENT_HPP_INCLUDE