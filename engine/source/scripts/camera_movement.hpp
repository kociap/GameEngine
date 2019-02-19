#include "components/behaviour_component.hpp"

class Camera_Movement : public Behaviour_Component {
public:
    using Behaviour_Component::Behaviour_Component;

    Vector3 camera_side = Vector3::right;

    void update() override;
};