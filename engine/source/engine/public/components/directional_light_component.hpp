#ifndef ENGINE_COMPONENTS_DIRECITONAL_LIGHT_COMPONENT_HPP_INCLUDE
#define ENGINE_COMPONENTS_DIRECITONAL_LIGHT_COMPONENT_HPP_INCLUDE

#include "color.hpp"
#include "math/vector3.hpp"

class Directional_Light_Component {
public:
    Vector3 direction = -Vector3::up;
    Color color = Color::white;
    float intensity = 1.0f;
    bool dynamic = false;
};

#endif // !ENGINE_COMPONENTS_DIRECITONAL_LIGHT_COMPONENT_HPP_INCLUDE