#ifndef ENGINE_ECS_COMPONENT_REGISTRY_HPP_INCLUDE
#define ENGINE_ECS_COMPONENT_REGISTRY_HPP_INCLUDE

#include "serialization.hpp"
#include "type_family.hpp"

#include <unordered_map>

class Component_Container_Base;

class Component_Registry {
public:
    Component_Registry();

    void serialize_component_container(Type_Family::family_t identifier, std::ostream&, Component_Container_Base*) const;
    void deserialize_component_container(Type_Family::family_t identifier, std::istream&, Component_Container_Base*&) const;

private:
    using serialize_func = void (*)(std::ostream&, Component_Container_Base*);
    using deserialize_func = void (*)(std::istream&, Component_Container_Base*&);
    struct Serialization_Funcs {
        serialize_func serialize;
        deserialize_func deserialize;
    };

    std::unordered_map<Type_Family::family_t, Serialization_Funcs> serialization_funcs;
};

#endif // !ENGINE_ECS_COMPONENT_REGISTRY_HPP_INCLUDE
