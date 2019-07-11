#ifndef ENGINE_COMPONENTS_POINT_LIGHT_COMPONENT_HPP_INCLUDE
#define ENGINE_COMPONENTS_POINT_LIGHT_COMPONENT_HPP_INCLUDE

#include <class_macros.hpp>
#include <color.hpp>
#include <math/vector3.hpp>
#include <serialization/serialization.hpp>

class COMPONENT Point_Light_Component {
public:
    Color color = Color::white;
    float intensity = 1.0f;
    bool dynamic = false;
};

DEFAULT_SERIALIZABLE(Point_Light_Component);
DEFAULT_DESERIALIZABLE(Point_Light_Component);

#endif // !ENGINE_COMPONENTS_POINT_LIGHT_COMPONENT_HPP_INCLUDE