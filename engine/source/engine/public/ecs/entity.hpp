#ifndef ENGINE_ENTITY_HPP_INCLUDE
#define ENGINE_ENTITY_HPP_INCLUDE

#include <cstdint>
#include <serialization/serialization.hpp>

namespace anton_engine {
    class Entity {
    public:
        using id_type = uint64_t;
        id_type id;
    };

    constexpr Entity null_entity{static_cast<Entity::id_type>(-1)};

    uint64_t entity_index(Entity);
    uint64_t entity_generation(Entity);

    bool operator==(Entity const&, Entity const&);
    bool operator!=(Entity const&, Entity const&);

    void swap(Entity&, Entity&);
} // namespace anton_engine

ANTON_DEFAULT_SERIALIZABLE(anton_engine::Entity);
ANTON_DEFAULT_DESERIALIZABLE(anton_engine::Entity);

#endif // !ENGINE_ENTITY_HPP_INCLUDE
