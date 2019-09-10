#ifndef ENGINE_COMPONENTS_DIRECITONAL_LIGHT_COMPONENT_HPP_INCLUDE
#define ENGINE_COMPONENTS_DIRECITONAL_LIGHT_COMPONENT_HPP_INCLUDE

#include <class_macros.hpp>
#include <color.hpp>
#include <math/vector3.hpp>
#include <serialization/serialization.hpp>

namespace anton_engine {
    class COMPONENT Directional_Light_Component {
    public:
        Vector3 direction = -Vector3::up;
        Color color = Color::white;
        float intensity = 1.0f;
        bool dynamic = false;
    };
} // namespace anton_engine

ANTON_DEFAULT_SERIALIZABLE(anton_engine::Directional_Light_Component);
ANTON_DEFAULT_DESERIALIZABLE(anton_engine::Directional_Light_Component);

#endif // !ENGINE_COMPONENTS_DIRECITONAL_LIGHT_COMPONENT_HPP_INCLUDE
