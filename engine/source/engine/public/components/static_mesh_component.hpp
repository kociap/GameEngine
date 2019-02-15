#ifndef ENGINE_COMPONENTS_STATIC_MESH_COMPONENT_HPP_INCLUDE
#define ENGINE_COMPONENTS_STATIC_MESH_COMPONENT_HPP_INCLUDE

#include "handle.hpp"
#include "renderable_component.hpp"

class Mesh;
class Shader;

class Static_Mesh_Component : public Renderable_Component {
public:
    Static_Mesh_Component(Game_Object&);
    using Renderable_Component::Renderable_Component;
    /*Static_Mesh_Component() = delete;
    Static_Mesh_Component(Static_Mesh_Component const&) = delete;
    Static_Mesh_Component(Static_Mesh_Component&&) noexcept;
    Static_Mesh_Component& operator=(Static_Mesh_Component const&) = delete;
    Static_Mesh_Component& operator=(Static_Mesh_Component&&) noexcept = delete;
    ~Static_Mesh_Component();*/


    void set_mesh(Handle<Mesh>);
    void set_shader(Handle<Shader>);

public:
    Handle<Mesh> mesh_handle;
    Handle<Shader> shader_handle;
};

#endif // !ENGINE_COMPONENTS_STATIC_MESH_COMPONENT_HPP_INCLUDE