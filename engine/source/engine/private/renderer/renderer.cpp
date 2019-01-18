#include "renderer/renderer.hpp"
namespace renderer {
    void Renderer::update() {}

    Handle<Renderer> Renderer::register_component() {
        return Handle<Renderer>(0);
    }

    void Renderer::unregister_component(Handle<Renderer>) {}

    void Renderer::set_material(Handle<Renderer>, Handle<Material>) {}

    void Renderer::set_mesh(Handle<Renderer>, Handle<Mesh>) {}

    void Renderer::set_local_to_world_transform(Handle<Renderer>, Matrix4 const&) {}
} // namespace renderer
