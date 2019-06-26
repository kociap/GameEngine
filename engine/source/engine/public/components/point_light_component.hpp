#ifndef ENGINE_COMPONENTS_POINT_LIGHT_COMPONENT_HPP_INCLUDE
#define ENGINE_COMPONENTS_POINT_LIGHT_COMPONENT_HPP_INCLUDE

#include <class_macros.hpp>
#include <color.hpp>
#include <math/vector3.hpp>
#include <serialization.hpp>

class COMPONENT Point_Light_Component {
public:
    Color color = Color::white;
    float intensity = 1.0f;
    bool dynamic = false;
};

template <>
struct serialization::use_default_deserialize<Point_Light_Component>: std::true_type {};

#endif // !ENGINE_COMPONENTS_POINT_LIGHT_COMPONENT_HPP_INCLUDE