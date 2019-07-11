#include <ecs/ecs.hpp>
#include <memory/memory.hpp>

ECS::~ECS() {
    for (auto& container_data: containers) {
        memory::destruct(container_data.container);
        container_data.container = nullptr;
    }
}

namespace serialization {
    void serialize(Binary_Output_Archive& archive, ECS const& ecs) {
        serialization::serialize(archive, ecs.entities);
        archive.write(ecs.containers.size());
        for (auto const data: ecs.containers) {
            archive.write(data.family);
            ecs.registry.serialize_component_container(data.family, archive, data.container);
        }
    }

    void deserialize(Binary_Input_Archive& archive, ECS& ecs) {
        serialization::deserialize(archive, ecs.entities);
        uint64_t containers_count;
        archive.read(containers_count);
        ecs.containers.resize(containers_count);
        for (auto& data: ecs.containers) {
            archive.read(data.family);
            ecs.registry.deserialize_component_container(data.family, archive, data.container);
        }
    }
} // namespace serialization