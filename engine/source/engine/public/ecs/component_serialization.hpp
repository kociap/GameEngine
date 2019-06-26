#ifndef SERIALIZATION_FUNCS_HPP_INCLUDE
#define SERIALIZATION_FUNCS_HPP_INCLUDE

#include <containers/vector.hpp>
#include <dll_build_config.hpp>
#include <type_family.hpp>

class Component_Container_Base;

using serialize_func = void (*)(std::ostream&, Component_Container_Base*);
using deserialize_func = void (*)(std::istream&, Component_Container_Base*&);

struct Component_Serialization_Funcs {
    Type_Family::family_t identifier;
    serialize_func serialize;
    deserialize_func deserialize;
};

using get_component_serialization_funcs_t = containers::Vector<Component_Serialization_Funcs>& (*)();

DLL_API extern get_component_serialization_funcs_t get_component_serialization_funcs;

#endif // !SERIALIZATION_FUNCS_HPP_INCLUDE
