#ifndef ENGINE_MESH_MESH_MANAGER_HPP_INCLUDE
#define ENGINE_MESH_MESH_MANAGER_HPP_INCLUDE

#include "containers/vector.hpp"
#include "handle.hpp"
#include "mesh.hpp"

class Mesh_Manager {
public:
    Handle<Mesh> add(Mesh&&);
    Mesh& get(Handle<Mesh> const&);
    void remove(Handle<Mesh> const&);

private:
    containers::Vector<Mesh> meshes;
};

#endif // !ENGINE_MESH_MESH_MANAGER_HPP_INCLUDE