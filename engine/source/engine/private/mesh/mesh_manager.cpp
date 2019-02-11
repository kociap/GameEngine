#include "mesh/mesh_manager.hpp"

Handle<Mesh> Mesh_Manager::add(Mesh&& mesh) {
    Handle<Mesh> handle(mesh.id.value());
    meshes.add(std::move(mesh));
    return handle;
}

Mesh& Mesh_Manager::get(Handle<Mesh> const& handle) {
    for (Mesh& mesh : meshes) {
        if (mesh.id.value() == handle.value) {
            return mesh;
		}
	}

	throw std::invalid_argument("");
}

void Mesh_Manager::remove(Handle<Mesh> const& handle) {
    Swapping_Pool<Mesh>::size_t i = 0;
    for (Mesh& mesh : meshes) {
        if (handle.value == mesh.id.value()) {
            meshes.remove_unchecked(i);
            break;
		}
        ++i;
	}
}