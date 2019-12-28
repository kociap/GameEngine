#include <anton_stl/vector.hpp>
#include <ecs/system.hpp>

namespace anton_engine {
    using create_systems_type = anton_stl::Vector<System*> (*)();
    ENGINE_API extern create_systems_type create_systems;

    void init_systems();
    void update_systems();
} // namespace anton_engine
