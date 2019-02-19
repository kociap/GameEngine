#ifndef ENGINE_COMPONENTS_BEHAVIOUR_COMPONENT_HPP_INCLUDE
#define ENGINE_COMPONENTS_BEHAVIOUR_COMPONENT_HPP_INCLUDE

#include "base_component.hpp"
#include "entity.hpp"
#include "transform.hpp"

class Behaviour_Component : public Base_Component {
public:
    using Base_Component::Base_Component;

    virtual void update() {}
};

#endif // !ENGINE_COMPONENTS_BEHAVIOUR_COMPONENT_HPP_INCLUDE