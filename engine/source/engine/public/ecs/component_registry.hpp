#ifndef ENGINE_ECS_COMPONENT_REGISTRY_HPP_INCLUDE
#define ENGINE_ECS_COMPONENT_REGISTRY_HPP_INCLUDE

#include <serialization/serialization.hpp>
#include <serialization/archives/binary.hpp>
#include <type_family.hpp>

#include <unordered_map>

class Component_Container_Base;

class Component_Registry {
public:
    void serialize_component_container(Type_Family::family_t identifier, serialization::Binary_Output_Archive&, Component_Container_Base*) const;
    void deserialize_component_container(Type_Family::family_t identifier, serialization::Binary_Input_Archive&, Component_Container_Base*&) const;
};

#endif // !ENGINE_ECS_COMPONENT_REGISTRY_HPP_INCLUDE
