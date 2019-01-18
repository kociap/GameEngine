#ifndef ENGINE_COMPONENTS_STATIC_MESH_COMPONENT_HPP_INCLUDE
#define ENGINE_COMPONENTS_STATIC_MESH_COMPONENT_HPP_INCLUDE

#include "handle.hpp"
#include "material.hpp"
#include "mesh.hpp"
#include "renderable_component.hpp"
#include <vector>

class Static_Mesh_Component : public Renderable_Component {
private:
    Handle<Mesh> mesh_handle;
    Handle<Material> material_handle;

public:
    using Renderable_Component::Renderable_Component;

    void set_mesh(Handle<Mesh>);
    void set_material(Handle<Material>);
};

#endif // !ENGINE_COMPONENTS_STATIC_MESH_COMPONENT_HPP_INCLUDE