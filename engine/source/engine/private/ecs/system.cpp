#include <ecs/system.hpp>
#include <ecs/system_management.hpp>

#include <logging.hpp>

namespace anton_engine {
    static anton_stl::Vector<System*> systems;

    // From game dll
    create_systems_type create_systems = nullptr;

    void init_systems() {
        systems = create_systems();
    }

    void update_systems() {
        for (System* system: systems) {
            system->update();
        }
    }
} // namespace anton_engine
