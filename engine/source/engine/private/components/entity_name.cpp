#include <components/entity_name.hpp>

namespace anton_engine::serialization {
    void serialize(Binary_Output_Archive& out, Entity_Name const& name) {
        serialization::serialize(out, name.name);
    }

    void deserialize(Binary_Input_Archive& in, Entity_Name& name) {
        serialization::deserialize(in, name.name);
    }
} // namespace anton_engine::serialization
