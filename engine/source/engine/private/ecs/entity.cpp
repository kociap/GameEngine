#include "ecs/entity.hpp"

bool operator==(Entity const& a, Entity const& b) {
    return a.id == b.id;
}

bool operator!=(Entity const& a, Entity const& b) {
    return a.id != b.id;
}