#ifndef COMPONENT_SERIALIZATION_FUNCS_HPP_INCLUDE
#define COMPONENT_SERIALIZATION_FUNCS_HPP_INCLUDE

#include <component_serialization_funcs.hpp>
#include <containers/vector.hpp>
#include <dll_build_config.hpp>
#include <ecs/component_serialization.hpp>

NO_NAME_MANGLE DLL_API containers::Vector<Component_Serialization_Funcs>& get_component_serialization_functions();

#endif // !COMPONENT_SERIALIZATION_FUNCS_HPP_INCLUDE
