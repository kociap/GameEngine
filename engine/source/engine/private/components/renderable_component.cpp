#include "components/renderable_component.hpp"

#include "renderer/renderer.hpp"

// extern renderer::Renderer g_renderer;

Renderable_Component::Renderable_Component(Game_Object& go) : Base_Component(go), handle(0) {
    // handle = g_renderer.register_component();
}

Renderable_Component::~Renderable_Component() {
    // g_renderer.unregister_component(handle);
}

void Renderable_Component::set_material(Handle<Material> mat) {
    // g_renderer.set_material(handle, mat);
}

void Renderable_Component::set_mesh(Handle<Mesh> mesh) {
    // g_renderer.set_mesh(handle, mesh);
}

void Renderable_Component::set_transform(Matrix4 const& mat) {
    // g_renderer.set_local_to_world_transform(handle, mat);
}
