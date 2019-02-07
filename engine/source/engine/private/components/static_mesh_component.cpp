#include "components/static_mesh_component.hpp"

#include "components/transform.hpp"
#include "containers/pool.hpp"
#include "mesh.hpp"
#include "shader.hpp"


void Static_Mesh_Component::render() {
    /*Shader& shader = g_shader_manager.get(shader_handle);
    shader.use();
    Matrix4 model_transform(transform.to_matrix());
    shader.set_matrix4("", model_transform);
    Mesh& mesh = g_mesh_manager.get(mesh_handle);
    mesh.draw(shader);*/
}

//void Static_Mesh_Component::set_mesh(Handle<Mesh> mesh) {
//    mesh_handle = std::move(mesh);
//}
//
//void Static_Mesh_Component::set_shader(Handle<Shader> mat) {
//    shader_handle = std::move(mat);
//}
