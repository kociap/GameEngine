#ifndef ENGINE_COMPONENTS_SPOTLIGHT_LIGHT_COMPONENT_HPP_INCLUDE
#define ENGINE_COMPONENTS_SPOTLIGHT_LIGHT_COMPONENT_HPP_INCLUDE

#include <class_macros.hpp>
#include <color.hpp>
#include <math/vector3.hpp>
#include <serialization/serialization.hpp>

namespace anton_engine {
    class COMPONENT Spot_Light_Component {
    public:
        Vector3 direction = -Vector3::up;
        Color color = Color::white;
        float cutoff_angle = 25.0f;
        float blend_angle = 20.0f;
        float intensity = 1.0f;
        bool dynamic = false;
    };
} // namespace anton_engine

ANTON_DEFAULT_SERIALIZABLE(anton_engine::Spot_Light_Component);

#endif // !ENGINE_COMPONENTS_SPOTLIGHT_LIGHT_COMPONENT_HPP_INCLUDE
