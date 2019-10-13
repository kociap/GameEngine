#ifndef ENGINE_COMPONENTS_ENTITY_NAME_HPP_INCLUDE
#define ENGINE_COMPONENTS_ENTITY_NAME_HPP_INCLUDE

#include <anton_stl/string.hpp>
#include <class_macros.hpp>
#include <serialization/archives/binary.hpp>
#include <serialization/serialization.hpp>

namespace anton_engine {
    class COMPONENT Entity_Name {
    public:
        anton_stl::String name;
    };

    inline void serialize(serialization::Binary_Output_Archive& out, Entity_Name const& name) {
        serialize(out, name.name);
    }

    inline void deserialize(serialization::Binary_Input_Archive& in, Entity_Name& name) {
        deserialize(in, name.name);
    }
} // namespace anton_engine

#endif // !ENGINE_COMPONENTS_ENTITY_NAME_HPP_INCLUDE
