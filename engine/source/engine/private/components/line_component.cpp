#include "components/line_component.hpp"

#include <utility>

Line_Component::Line_Component(Game_Object& go) : Renderable_Component(go), mesh_handle(0), shader_handle(0) {}

void Line_Component::set_mesh(Handle<Mesh> mesh) {
    mesh_handle = std::move(mesh);
}

void Line_Component::set_shader(Handle<Shader> handle) {
    shader_handle = std::move(handle);
}