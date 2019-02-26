#ifndef SCRIPTS_DEBUG_HOTKEYS_HPP_INCLUDE
#define SCRIPTS_DEBUG_HOTKEYS_HPP_INCLUDE

#include "components/behaviour_component.hpp"

class Debug_Hotkeys : public Behaviour_Component {
public:
    using Behaviour_Component::Behaviour_Component;

    float reload_old = 0.0f;
    float show_shadow_map_old = 0.0f;

    void update() override;
};

#endif // !SCRIPTS_DEBUG_HOTKEYS_HPP_INCLUDE
