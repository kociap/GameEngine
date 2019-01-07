#ifndef ENGINE_COMPONENTS_DIRECITONAL_LIGHT_COMPONENT_HPP_INCLUDE
#define ENGINE_COMPONENTS_DIRECITONAL_LIGHT_COMPONENT_HPP_INCLUDE

#include "base_component.hpp"
#include "color.hpp"
#include "math/vector3.hpp"
#include "shader.hpp"

namespace component {
    class Directional_Light_Component : public Base_Component {
    public:
        Vector3 position;
        Vector3 direction;
        Color color;

        void draw(Shader&);
    };
} // namespace component

#endif // !ENGINE_COMPONENTS_DIRECITONAL_LIGHT_COMPONENT_HPP_INCLUDE