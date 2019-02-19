#include "components/base_component.hpp"

Base_Component::Base_Component(Entity const& entity) : _entity(entity) {}

Base_Component::~Base_Component() {}

Entity const& Base_Component::get_entity() const {
    return _entity;
}
