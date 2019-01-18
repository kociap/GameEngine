#ifndef ENGINE_COMPONENTS_TRANSFORM_COMPONENT_HPP_INCLUDE
#define ENGINE_COMPONENTS_TRANSFORM_COMPONENT_HPP_INCLUDE

#include "base_component.hpp"
#include "math/matrix4.hpp"
#include "math/vector3.hpp"
#include "math/quaternion.hpp"

class Transform : public Base_Component {
public:
    Quaternion rotation;
    Vector3 position;
    Vector3 local_position;
    Vector3 local_scale;

public:
    using Base_Component::Base_Component;

    void translate(Vector3);
    void scale(Vector3);
	// axis to rotate about
	// angle in radians
    void rotate(Vector3 axis, float angle);
    Matrix4 to_matrix() const;
};

#endif // !ENGINE_COMPONENTS_TRANSFORM_COMPONENT_HPP_INCLUDE