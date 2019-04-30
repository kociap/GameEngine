#ifndef ENGINE_COMPONENTS_SPOTLIGHT_LIGHT_COMPONENT_HPP_INCLUDE
#define ENGINE_COMPONENTS_SPOTLIGHT_LIGHT_COMPONENT_HPP_INCLUDE

#include "color.hpp"
#include "math/vector3.hpp"

class Spot_Light_Component {
public:
    Vector3 direction = -Vector3::up;
    Color color = Color::white;
    float cutoff_angle = 25.0f;
    float blend_angle = 20.0f;
    float intensity = 1.0f;
    bool dynamic = false;
};

#endif // !ENGINE_COMPONENTS_SPOTLIGHT_LIGHT_COMPONENT_HPP_INCLUDE