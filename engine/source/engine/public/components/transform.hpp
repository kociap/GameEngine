#ifndef ENGINE_COMPONENTS_TRANSFORM_COMPONENT_HPP_INCLUDE
#define ENGINE_COMPONENTS_TRANSFORM_COMPONENT_HPP_INCLUDE

#include "base_component.hpp"
#include "math/vector3.hpp"

namespace component {
    class Transform : public Base_Component {
    public:
        Vector3 position;
        Vector3 scale;
    };
} // namespace component

#endif // !ENGINE_COMPONENTS_TRANSFORM_COMPONENT_HPP_INCLUDE