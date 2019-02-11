#ifndef ENGINE_COMPONENTS_STATIC_MESH_COMPONENT_HPP_INCLUDE
#define ENGINE_COMPONENTS_STATIC_MESH_COMPONENT_HPP_INCLUDE

#include "handle.hpp"
#include "renderable_component.hpp"

class Mesh;

class Static_Mesh_Component : public Renderable_Component {
public:
    Static_Mesh_Component(Game_Object&);

    void render(Shader&) override;

    void set_mesh(Handle<Mesh>);

private:
    Handle<Mesh> mesh_handle;
};

#endif // !ENGINE_COMPONENTS_STATIC_MESH_COMPONENT_HPP_INCLUDE