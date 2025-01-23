#pragma once

#include <anton/array.hpp>
#include <anton/typeid.hpp>
#include <core/serialization/archives/binary.hpp>

namespace anton_engine {
  class Component_Container_Base;

  using serialize_func = void (*)(serialization::Binary_Output_Archive&,
                                  Component_Container_Base const*);
  using deserialize_func = void (*)(serialization::Binary_Input_Archive&,
                                    Component_Container_Base*&);

  struct Component_Serialization_Funcs {
    u64 identifier;
    serialize_func serialize;
    deserialize_func deserialize;
  };

  using get_component_serialization_funcs_t =
    anton::Array<Component_Serialization_Funcs>& (*)();

  ENGINE_API extern get_component_serialization_funcs_t
    get_component_serialization_funcs;
} // namespace anton_engine
