#include "components/renderable_component.hpp"

#include "engine.hpp"
#include "renderer.hpp"

Renderable_Component::Renderable_Component(Game_Object& go) : Base_Component(go) {
    Engine::get_renderer().register_component(this);
}

Renderable_Component::~Renderable_Component() {
    Engine::get_renderer().unregister_component(this);
}
