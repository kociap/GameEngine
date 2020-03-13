#include <engine/ecs/system.hpp>
#include <engine/ecs/system_management.hpp>

#include <core/logging.hpp>

namespace anton_engine {
    static atl::Vector<System*> systems;

    // From game dll
    create_systems_type create_systems = nullptr;

    void init_systems() {
        systems = create_systems();
    }

    void start_systems() {
        for (System* system: systems) {
            system->start();
        }
    }

    void update_systems() {
        for (System* system: systems) {
            system->update();
        }
    }
} // namespace anton_engine
