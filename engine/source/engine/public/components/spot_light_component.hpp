#ifndef ENGINE_COMPONENTS_SPOTLIGHT_LIGHT_COMPONENT_HPP_INCLUDE
#define ENGINE_COMPONENTS_SPOTLIGHT_LIGHT_COMPONENT_HPP_INCLUDE

#include "base_component.hpp"
#include "color.hpp"
#include "math/vector3.hpp"

namespace component {
    class Spot_Light_Component : public Base_Component {
    public:
        Vector3 position;
        Vector3 direction;
        Color color;
        float cutoff_angle;
        float blend_angle;
        float intensity;
    };
} // namespace component

#endif // !ENGINE_COMPONENTS_SPOTLIGHT_LIGHT_COMPONENT_HPP_INCLUDE