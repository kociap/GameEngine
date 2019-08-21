#include <anton_stl/utility.hpp>
#include <ecs/entity.hpp>

bool operator==(Entity const& a, Entity const& b) {
    return a.id == b.id;
}

bool operator!=(Entity const& a, Entity const& b) {
    return a.id != b.id;
}

void swap(Entity& e1, Entity& e2) {
    anton_stl::swap(e1.id, e2.id);
}
