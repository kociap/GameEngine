#ifndef COMPONENT_SERIALIZATION_FUNCS_HPP_INCLUDE
#define COMPONENT_SERIALIZATION_FUNCS_HPP_INCLUDE

#include <anton_stl/vector.hpp>
#include <component_serialization_funcs.hpp>
#include <ecs/component_serialization.hpp>

namespace anton_engine {
    extern "C" GAME_API anton_stl::Vector<Component_Serialization_Funcs>& get_component_serialization_functions();
}

#endif // !COMPONENT_SERIALIZATION_FUNCS_HPP_INCLUDE
