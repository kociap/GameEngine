#include "entity.hpp"

#include "guid.hpp"

#include "engine.hpp"
#include "entity_manager.hpp"

Entity Entity::instantiate() {
    Guid new_entity_guid;
    Entity_Manager& entity_manager = Engine::get_entity_manager();
    Entity new_entity = entity_manager.emplace(Entity(new_entity_guid));
    return new_entity;
}

Entity Entity::instantiate(Entity const& entity) {
    Entity new_entity = instantiate();
	// TODO copy components
    return new_entity;
}

Entity::Entity(Guid const& guid) : id(guid) {}

bool operator==(Entity const& a, Entity const& b) {
    return a.id == b.id;
}