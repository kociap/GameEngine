#ifndef ENGINE_COMPONENTS_RENDERABLE_COMPONENT_HPP_INCLUDE
#define ENGINE_COMPONENTS_RENDERABLE_COMPONENT_HPP_INCLUDE

#include "base_component.hpp"
#include "handle.hpp"
#include "shader.hpp"

class Renderable_Component : public Base_Component {
public:
    Renderable_Component(Game_Object&);
    Renderable_Component(Renderable_Component&&) = default;
    Renderable_Component& operator=(Renderable_Component&&) = default;
    virtual ~Renderable_Component();

    virtual void render() = 0;
};

#endif // !ENGINE_COMPONENTS_RENDERABLE_COMPONENT_HPP_INCLUDE