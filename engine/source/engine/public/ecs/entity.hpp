#ifndef ENGINE_ENTITY_HPP_INCLUDE
#define ENGINE_ENTITY_HPP_INCLUDE

#include <anton_stl/utility.hpp>
#include <cstdint>
#include <serialization/serialization.hpp>

namespace anton_engine {
    class Entity {
    public:
        uint64_t id;
    };

    constexpr Entity null_entity{static_cast<uint64_t>(-1)};

    [[nodiscard]] constexpr uint64_t entity_index(Entity const entity) {
        return entity.id & 0xFFFFFFFF;
    }

    [[nodiscard]] constexpr uint64_t entity_generation(Entity const entity) {
        return (entity.id >> 32) & 0xFFFFFFFF;
    }

    [[nodiscard]] constexpr bool operator==(Entity const& lhs, Entity const& rhs) {
        return lhs.id == rhs.id;
    }

    [[nodiscard]] constexpr bool operator!=(Entity const& lhs, Entity const& rhs) {
        return lhs.id != rhs.id;
    }

    constexpr void swap(Entity& e1, Entity& e2) {
        anton_stl::swap(e1.id, e2.id);
    }
} // namespace anton_engine

ANTON_DEFAULT_SERIALIZABLE(anton_engine::Entity);

#endif // !ENGINE_ENTITY_HPP_INCLUDE
