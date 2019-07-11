#include <debug_macros.hpp>
#include <ecs/component_registry.hpp>
#include <ecs/component_serialization.hpp>
#include <ecs/ecs.hpp>

#include <algorithm>

void Component_Registry::serialize_component_container(Type_Family::family_t identifier, serialization::Binary_Output_Archive& archive, Component_Container_Base* container) const {
    GE_assert(get_component_serialization_funcs != nullptr, "Function get_component_serialization_funcs has not been loaded");
    auto& serialization_funcs = get_component_serialization_funcs();
    auto iter = std::find_if(serialization_funcs.begin(), serialization_funcs.end(),
                             [identifier](Component_Serialization_Funcs const& funcs) { return funcs.identifier == identifier; });
    GE_assert(iter != serialization_funcs.end(), "Identifier was not found in the component registry");
    iter->serialize(archive, container);
}

void Component_Registry::deserialize_component_container(Type_Family::family_t identifier, serialization::Binary_Input_Archive& archive, Component_Container_Base*& container) const {
    GE_assert(get_component_serialization_funcs != nullptr, "Function get_component_serialization_funcs has not been loaded");
    auto& serialization_funcs = get_component_serialization_funcs();
    auto iter = std::find_if(serialization_funcs.begin(), serialization_funcs.end(),
                             [identifier](Component_Serialization_Funcs const& funcs) { return funcs.identifier == identifier; });
    GE_assert(iter != serialization_funcs.end(), "Identifier was not found in the component registry");
    iter->deserialize(archive, container);
}