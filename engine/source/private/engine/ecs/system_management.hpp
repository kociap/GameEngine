#include <core/stl/vector.hpp>
#include <engine/ecs/system.hpp>

namespace anton_engine {
    using create_systems_type = anton_stl::Vector<System*> (*)();
    ENGINE_API extern create_systems_type create_systems;

    void init_systems();
    void start_systems();
    void update_systems();
} // namespace anton_engine
