#ifndef ENGINE_COMPONENTS_POINT_LIGHT_COMPONENT_HPP_INCLUDE
#define ENGINE_COMPONENTS_POINT_LIGHT_COMPONENT_HPP_INCLUDE

#include "base_component.hpp"
#include "color.hpp"
#include "math/vector3.hpp"

class Point_Light_Component : public Base_Component {
public:
    Vector3 position;
    Color color;
};

#endif // !ENGINE_COMPONENTS_POINT_LIGHT_COMPONENT_HPP_INCLUDE