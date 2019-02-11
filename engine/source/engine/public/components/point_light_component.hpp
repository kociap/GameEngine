#ifndef ENGINE_COMPONENTS_POINT_LIGHT_COMPONENT_HPP_INCLUDE
#define ENGINE_COMPONENTS_POINT_LIGHT_COMPONENT_HPP_INCLUDE

#include "base_component.hpp"
#include "color.hpp"
#include "math/vector3.hpp"

class Point_Light_Component : public Base_Component {
public:
    using Base_Component::Base_Component;

    Color color = Color::white;
    float intensity = 1.0f;
};

#endif // !ENGINE_COMPONENTS_POINT_LIGHT_COMPONENT_HPP_INCLUDE