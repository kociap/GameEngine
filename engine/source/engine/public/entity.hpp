#ifndef ENGINE_ENTITY_HPP_INCLUDE
#define ENGINE_ENTITY_HPP_INCLUDE

#include "id.hpp"

class Guid;

class Entity {
public:
    ID id;

public:
    // Construct new entity
    static Entity instantiate();
    // Copy entity and its components
	// TODO currently doesn't copy components
    static Entity instantiate(Entity const&);
    static void destroy(Entity&);

private:
    Entity(Guid const&);
};

bool operator==(Entity const&, Entity const&);

#endif // !ENGINE_ENTITY_HPP_INCLUDE