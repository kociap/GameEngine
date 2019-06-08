#ifndef ENGINE_ENTITY_HPP_INCLUDE
#define ENGINE_ENTITY_HPP_INCLUDE

#include "serialization.hpp"
#include <cstdint>

class Entity {
public:
    using id_type = uint64_t;
    id_type id;
};

constexpr Entity null_entity{static_cast<Entity::id_type>(-1)};

bool operator==(Entity const&, Entity const&);
bool operator!=(Entity const&, Entity const&);

void swap(Entity&, Entity&);

template <>
struct serialization::use_default_deserialize<Entity>: std::true_type {};

#endif // !ENGINE_ENTITY_HPP_INCLUDE