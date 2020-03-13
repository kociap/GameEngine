#ifndef ENGINE_COMPONENTS_HIERARCHY_HPP_INCLUDE
#define ENGINE_COMPONENTS_HIERARCHY_HPP_INCLUDE

#include <core/atl/fixed_array.hpp>
#include <core/class_macros.hpp>
#include <engine/ecs/entity.hpp>
#include <core/serialization/serialization.hpp>

namespace anton_engine {
    class COMPONENT Hierarchy {
    public:
        Entity parent;
        // TODO: Right now supports only 16 children.
        atl::Fixed_Array<Entity, 16> children;
    };
} // namespace anton_engine

ANTON_DEFAULT_SERIALIZABLE(anton_engine::Hierarchy);

#endif // !ENGINE_COMPONENTS_HIERARCHY_HPP_INCLUDE
