#include <anton_stl/utility.hpp>
#include <ecs/entity.hpp>

namespace anton_engine {
    // TODO: Consider making constexpr
    uint64_t entity_index(Entity entity) {
        return entity.id & 0xFFFFFFFF;
    }

    uint64_t entity_generation(Entity entity) {
        return (entity.id >> 32) & 0xFFFFFFFF;
    }

    bool operator==(Entity const& a, Entity const& b) {
        return a.id == b.id;
    }

    bool operator!=(Entity const& a, Entity const& b) {
        return a.id != b.id;
    }

    void swap(Entity& e1, Entity& e2) {
        anton_stl::swap(e1.id, e2.id);
    }
} // namespace anton_engine
