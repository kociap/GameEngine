#ifndef ENGINE_COMPONENTS_DIRECITONAL_LIGHT_COMPONENT_HPP_INCLUDE
#define ENGINE_COMPONENTS_DIRECITONAL_LIGHT_COMPONENT_HPP_INCLUDE

#include "color.hpp"
#include "math/vector3.hpp"
#include "serialization.hpp"

class Directional_Light_Component {
public:
    Vector3 direction = -Vector3::up;
    Color color = Color::white;
    float intensity = 1.0f;
    bool dynamic = false;
};

template <>
struct serialization::use_default_deserialize<Directional_Light_Component>: std::true_type {};

#endif // !ENGINE_COMPONENTS_DIRECITONAL_LIGHT_COMPONENT_HPP_INCLUDE