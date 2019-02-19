#ifndef ENGINE_COMPONENTS_RENDERABLE_COMPONENT_HPP_INCLUDE
#define ENGINE_COMPONENTS_RENDERABLE_COMPONENT_HPP_INCLUDE

#include "base_component.hpp"

class Shader;

class Renderable_Component : public Base_Component {
public:
    Renderable_Component() = delete;
    Renderable_Component(Entity const&);
    Renderable_Component(Renderable_Component const&) = delete;
    Renderable_Component(Renderable_Component&&) noexcept = default;
    Renderable_Component& operator=(Renderable_Component const&) = delete;
    Renderable_Component& operator=(Renderable_Component&&) noexcept = default;
    virtual ~Renderable_Component(){};
};

#endif // !ENGINE_COMPONENTS_RENDERABLE_COMPONENT_HPP_INCLUDE