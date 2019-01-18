#include "components/static_mesh_component.hpp"

void Static_Mesh_Component::set_mesh(Handle<Mesh> mesh) {
    mesh_handle = std::move(mesh);
}

void Static_Mesh_Component::set_material(Handle<Material> mat) {
    material_handle = std::move(mat);
}
