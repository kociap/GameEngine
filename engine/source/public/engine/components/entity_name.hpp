#ifndef ENGINE_COMPONENTS_ENTITY_NAME_HPP_INCLUDE
#define ENGINE_COMPONENTS_ENTITY_NAME_HPP_INCLUDE

#include <core/atl/string.hpp>
#include <core/class_macros.hpp>
#include <core/serialization/archives/binary.hpp>
#include <core/serialization/serialization.hpp>

namespace anton_engine {
    class COMPONENT Entity_Name {
    public:
        atl::String name;
    };

    inline void serialize(serialization::Binary_Output_Archive& out, Entity_Name const& name) {
        serialize(out, name.name);
    }

    inline void deserialize(serialization::Binary_Input_Archive& in, Entity_Name& name) {
        deserialize(in, name.name);
    }
} // namespace anton_engine

#endif // !ENGINE_COMPONENTS_ENTITY_NAME_HPP_INCLUDE
