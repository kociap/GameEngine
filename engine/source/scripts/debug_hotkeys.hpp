#include "components/behaviour_component.hpp"

class Debug_Hotkeys : public Behaviour_Component {
public:
    using Behaviour_Component::Behaviour_Component;

    float reload_old = 0;

    void update() override;
};