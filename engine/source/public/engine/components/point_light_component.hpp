#ifndef ENGINE_COMPONENTS_POINT_LIGHT_COMPONENT_HPP_INCLUDE
#define ENGINE_COMPONENTS_POINT_LIGHT_COMPONENT_HPP_INCLUDE

#include <core/class_macros.hpp>
#include <core/color.hpp>
#include <core/math/vector3.hpp>
#include <core/serialization/serialization.hpp>

namespace anton_engine {
    class COMPONENT Point_Light_Component {
    public:
        Color color = Color::white;
        float intensity = 1.0f;
        bool dynamic = false;
    };
} // namespace anton_engine

ANTON_DEFAULT_SERIALIZABLE(::anton_engine::Point_Light_Component)

#endif // !ENGINE_COMPONENTS_POINT_LIGHT_COMPONENT_HPP_INCLUDE
