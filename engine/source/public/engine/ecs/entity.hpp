#ifndef ENGINE_ECS_ENTITY_HPP_INCLUDE
#define ENGINE_ECS_ENTITY_HPP_INCLUDE

#include <core/atl/utility.hpp>
#include <core/types.hpp>
#include <core/serialization/serialization.hpp>

namespace anton_engine {
    class Entity {
    public:
        u64 id;
    };

    constexpr Entity null_entity{static_cast<u64>(-1)};

    [[nodiscard]] constexpr u64 entity_index(Entity const entity) {
        return entity.id & 0xFFFFFFFF;
    }

    [[nodiscard]] constexpr u64 entity_generation(Entity const entity) {
        return (entity.id >> 32) & 0xFFFFFFFF;
    }

    [[nodiscard]] constexpr bool operator==(Entity const& lhs, Entity const& rhs) {
        return lhs.id == rhs.id;
    }

    [[nodiscard]] constexpr bool operator!=(Entity const& lhs, Entity const& rhs) {
        return lhs.id != rhs.id;
    }

    constexpr void swap(Entity& e1, Entity& e2) {
        atl::swap(e1.id, e2.id);
    }
} // namespace anton_engine

ANTON_DEFAULT_SERIALIZABLE(anton_engine::Entity)

#endif // !ENGINE_ECS_ENTITY_HPP_INCLUDE
