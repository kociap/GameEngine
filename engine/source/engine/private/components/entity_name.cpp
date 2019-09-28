#include <components/entity_name.hpp>

namespace anton_engine {
    void serialize(serialization::Binary_Output_Archive& out, Entity_Name const& name) {
        serialize(out, name.name);
    }

    void deserialize(serialization::Binary_Input_Archive& in, Entity_Name& name) {
        deserialize(in, name.name);
    }
} // namespace anton_engine
