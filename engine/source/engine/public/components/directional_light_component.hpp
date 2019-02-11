#ifndef ENGINE_COMPONENTS_DIRECITONAL_LIGHT_COMPONENT_HPP_INCLUDE
#define ENGINE_COMPONENTS_DIRECITONAL_LIGHT_COMPONENT_HPP_INCLUDE

#include "base_component.hpp"
#include "color.hpp"
#include "math/vector3.hpp"

class Directional_Light_Component : public Base_Component {
public:
    using Base_Component::Base_Component;

    Vector3 direction = -Vector3::up;
    Color color = Color::white;
	float intensity = 1.0f;
};

#endif // !ENGINE_COMPONENTS_DIRECITONAL_LIGHT_COMPONENT_HPP_INCLUDE