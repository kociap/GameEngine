#ifndef ENGINE_COMPONENTS_STATIC_MESH_COMPONENT_HPP_INCLUDE
#define ENGINE_COMPONENTS_STATIC_MESH_COMPONENT_HPP_INCLUDE

#include "handle.hpp"
#include "renderable_component.hpp"

class Mesh;
class Shader;
class Entity;

class Static_Mesh_Component : public Renderable_Component {
public:
    Static_Mesh_Component(Entity const&);
    using Renderable_Component::Renderable_Component;

    Handle<Mesh> mesh_handle;
    Handle<Shader> shader_handle;
};

#endif // !ENGINE_COMPONENTS_STATIC_MESH_COMPONENT_HPP_INCLUDE