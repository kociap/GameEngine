#include "ecs/component_registry.hpp"
#include "debug_macros.hpp"
#include "ecs/ecs.hpp"

void Component_Registry::serialize_component_container(Type_Family::family_t identifier, std::ostream& archive, Component_Container_Base* container) const {
    GE_assert(serialization_funcs.find(identifier) != serialization_funcs.end(), "Identifier was not found in the component registry");
    auto func_ptr = serialization_funcs.at(identifier).serialize;
    func_ptr(archive, container);
}

void Component_Registry::deserialize_component_container(Type_Family::family_t identifier, std::istream& archive, Component_Container_Base*& container) const {
    GE_assert(serialization_funcs.find(identifier) != serialization_funcs.end(), "Identifier was not found in the component registry");
    auto func_ptr = serialization_funcs.at(identifier).deserialize;
    func_ptr(archive, container);
}