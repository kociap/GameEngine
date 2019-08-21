#include <ecs/ecs.hpp>

#include <debug_macros.hpp>
#include <ecs/component_serialization.hpp>
#include <memory/memory.hpp>

#include <algorithm>

ECS::~ECS() {
    for (auto& container_data: containers) {
        memory::destruct(container_data.container);
        container_data.container = nullptr;
    }
}

void ECS::serialize_component_container(Type_Family::family_t identifier, serialization::Binary_Output_Archive& archive,
                                        Component_Container_Base const* container) const {
    GE_assert(get_component_serialization_funcs != nullptr, "Function get_component_serialization_funcs has not been loaded");
    auto& serialization_funcs = get_component_serialization_funcs();
    auto iter = std::find_if(serialization_funcs.begin(), serialization_funcs.end(),
                             [identifier](Component_Serialization_Funcs const& funcs) { return funcs.identifier == identifier; });
    GE_assert(iter != serialization_funcs.end(), "Identifier was not found in the component registry");
    iter->serialize(archive, container);
}

void ECS::deserialize_component_container(Type_Family::family_t identifier, serialization::Binary_Input_Archive& archive,
                                          Component_Container_Base*& container) {
    GE_assert(get_component_serialization_funcs != nullptr, "Function get_component_serialization_funcs has not been loaded");
    auto& serialization_funcs = get_component_serialization_funcs();
    auto iter = std::find_if(serialization_funcs.begin(), serialization_funcs.end(),
                             [identifier](Component_Serialization_Funcs const& funcs) { return funcs.identifier == identifier; });
    GE_assert(iter != serialization_funcs.end(), "Identifier was not found in the component registry");
    iter->deserialize(archive, container);
}

namespace serialization {
    void serialize(Binary_Output_Archive& archive, ECS const& ecs) {
        serialization::serialize(archive, ecs.entities);
        archive.write(ecs.containers.size());
        for (auto const& data: ecs.containers) {
            archive.write(data.family);
            ecs.serialize_component_container(data.family, archive, data.container);
        }
    }

    void deserialize(Binary_Input_Archive& archive, ECS& ecs) {
        serialization::deserialize(archive, ecs.entities);
        int64_t containers_count;
        archive.read(containers_count);
        ecs.containers.resize(containers_count);
        for (auto& data: ecs.containers) {
            archive.read(data.family);
            ecs.deserialize_component_container(data.family, archive, data.container);
        }
    }
} // namespace serialization
