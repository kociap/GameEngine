#include "components/component_system.hpp"

void Component_System::update_behaviours() {
    for (auto& [type_id, pool] : behaviour_components) {
        for (auto& behaviour_ptr : pool) {
            behaviour_ptr->update();
		}
	}
}