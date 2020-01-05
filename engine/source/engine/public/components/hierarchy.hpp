#ifndef ENGINE_COMPONENTS_HIERARCHY_HPP_INCLUDE
#define ENGINE_COMPONENTS_HIERARCHY_HPP_INCLUDE

#include <anton_stl/fixed_array.hpp>
#include <class_macros.hpp>
#include <ecs/entity.hpp>
#include <serialization/serialization.hpp>

namespace anton_engine {
    class COMPONENT Hierarchy {
    public:
        Entity parent;
        // TODO: Right now supports only 16 children.
        anton_stl::Fixed_Array<Entity, 16> children;
    };
} // namespace anton_engine

ANTON_DEFAULT_SERIALIZABLE(anton_engine::Hierarchy);

#endif // !ENGINE_COMPONENTS_HIERARCHY_HPP_INCLUDE
