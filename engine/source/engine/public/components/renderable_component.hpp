#ifndef ENGINE_COMPONENTS_RENDERABLE_COMPONENT_HPP_INCLUDE
#define ENGINE_COMPONENTS_RENDERABLE_COMPONENT_HPP_INCLUDE

#include "base_component.hpp"

class Shader;

class Renderable_Component : public Base_Component {
public:
    Renderable_Component(Game_Object&);
    Renderable_Component(Renderable_Component&&) noexcept = default;
    Renderable_Component& operator=(Renderable_Component&&) noexcept = default;
    virtual ~Renderable_Component();

	// TODO remove shader parameter. Temporarily here to simplify things in renderer until I build a proper shader manager
    virtual void render(Shader&) = 0;
};

#endif // !ENGINE_COMPONENTS_RENDERABLE_COMPONENT_HPP_INCLUDE