#ifndef COMPONENT_SERIALIZATION_FUNCS_HPP_INCLUDE
#define COMPONENT_SERIALIZATION_FUNCS_HPP_INCLUDE

#include <component_serialization_funcs.hpp>
#include <containers/vector.hpp>
#include <ecs/component_serialization.hpp>

extern "C" GAME_API containers::Vector<Component_Serialization_Funcs>& get_component_serialization_functions();

#endif // !COMPONENT_SERIALIZATION_FUNCS_HPP_INCLUDE
