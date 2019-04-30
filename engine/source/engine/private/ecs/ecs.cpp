#include "ecs/ecs.hpp"
#include "memory/memory.hpp"

ECS::~ECS() {
    for (auto& container_data: containers) {
        destruct(container_data.pointer);
        container_data.pointer = nullptr;
    }
}