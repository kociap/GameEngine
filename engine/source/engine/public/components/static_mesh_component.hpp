#ifndef ENGINE_COMPONENTS_STATIC_MESH_COMPONENT_HPP_INCLUDE
#define ENGINE_COMPONENTS_STATIC_MESH_COMPONENT_HPP_INCLUDE

#include "handle.hpp"
#include "mesh.hpp"
#include "renderable_component.hpp"
#include "shader.hpp"
#include <vector>

class Static_Mesh_Component : public Renderable_Component {

public:
    using Renderable_Component::Renderable_Component;

    void render() override;

    /*void set_mesh(Handle<Mesh>);
    void set_shader(Handle<Shader>);*/
};

#endif // !ENGINE_COMPONENTS_STATIC_MESH_COMPONENT_HPP_INCLUDE