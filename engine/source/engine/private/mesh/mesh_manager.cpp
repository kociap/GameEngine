#include "mesh/mesh_manager.hpp"

Handle<Mesh> Mesh_Manager::add(Mesh&& mesh) {
    Handle<Mesh> handle{mesh.id.value()};
    meshes.emplace_back(std::move(mesh));
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
    containers::Vector<Mesh>::size_type i = 0;
    for (Mesh& mesh : meshes) {
        if (handle.value == mesh.id.value()) {
            meshes.erase_unsorted_unchecked(i);
            break;
		}
        ++i;
	}
}