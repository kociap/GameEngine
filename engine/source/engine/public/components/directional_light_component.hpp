#ifndef ENGINE_COMPONENTS_DIRECITONAL_LIGHT_COMPONENT_HPP_INCLUDE
#define ENGINE_COMPONENTS_DIRECITONAL_LIGHT_COMPONENT_HPP_INCLUDE

#include <class_macros.hpp>
#include <color.hpp>
#include <math/vector3.hpp>
#include <serialization/serialization.hpp>

class COMPONENT Directional_Light_Component {
public:
    Vector3 direction = -Vector3::up;
    Color color = Color::white;
    float intensity = 1.0f;
    bool dynamic = false;
};

DEFAULT_SERIALIZABLE(Directional_Light_Component);
DEFAULT_DESERIALIZABLE(Directional_Light_Component);

#endif // !ENGINE_COMPONENTS_DIRECITONAL_LIGHT_COMPONENT_HPP_INCLUDE