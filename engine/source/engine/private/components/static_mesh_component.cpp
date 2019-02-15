#include "components/static_mesh_component.hpp"

#include "components/transform.hpp"
#include "containers/pool.hpp"
#include "engine.hpp"
#include "mesh/mesh.hpp"
#include "mesh/mesh_manager.hpp"

Static_Mesh_Component::Static_Mesh_Component(Game_Object& go) : Renderable_Component(go), mesh_handle(0), shader_handle(0) {}

void Static_Mesh_Component::set_mesh(Handle<Mesh> mesh) {
    mesh_handle = std::move(mesh);
}

void Static_Mesh_Component::set_shader(Handle<Shader> mat) {
    shader_handle = std::move(mat);
}
