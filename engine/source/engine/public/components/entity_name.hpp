#ifndef ENGINE_COMPONENTS_ENTITY_NAME_HPP_INCLUDE
#define ENGINE_COMPONENTS_ENTITY_NAME_HPP_INCLUDE

#include <anton_stl/string.hpp>
#include <class_macros.hpp>
#include <serialization/archives/binary.hpp>
#include <serialization/serialization.hpp>

class COMPONENT Entity_Name {
public:
    anton_stl::String name;
};

namespace serialization {
    void serialize(Binary_Output_Archive&, Entity_Name const&);
    void deserialize(Binary_Input_Archive&, Entity_Name&);
} // namespace serialization

#endif // !ENGINE_COMPONENTS_ENTITY_NAME_HPP_INCLUDE
