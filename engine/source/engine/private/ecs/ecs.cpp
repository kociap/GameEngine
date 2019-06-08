#include "ecs/ecs.hpp"
#include "memory/memory.hpp"

ECS::~ECS() {
    for (auto& container_data: containers) {
        memory::destruct(container_data.container);
        container_data.container = nullptr;
    }
}

namespace serialization {
    void serialize(std::ostream& archive, ECS const& ecs) {
        serialization::serialize(archive, ecs.entities);
        serialization::detail::write(archive, ecs.containers.size());
        for (auto const data: ecs.containers) {
            serialization::detail::write(archive, data.family);
            ecs.registry.serialize_component_container(data.family, archive, data.container);
        }
    }

    void deserialize(std::istream& archive, ECS& ecs) {
        serialization::deserialize(archive, ecs.entities);
        uint64_t containers_count;
        serialization::detail::read(archive, containers_count);
        ecs.containers.resize(containers_count);
        for (auto& data: ecs.containers) {
            serialization::detail::read(archive, data.family);
            ecs.registry.deserialize_component_container(data.family, archive, data.container);
        }
    }
} // namespace serialization