#include "ecs/entity.hpp"
#include "utility.hpp"

bool operator==(Entity const& a, Entity const& b) {
    return a.id == b.id;
}

bool operator!=(Entity const& a, Entity const& b) {
    return a.id != b.id;
}

void swap(Entity& e1, Entity& e2) {
    swap(e1.id, e2.id);
}