#ifndef ENGINE_COMPONENTS_RENDERABLE_COMPONENT_HPP_INCLUDE
#define ENGINE_COMPONENTS_RENDERABLE_COMPONENT_HPP_INCLUDE

#include "base_component.hpp"
#include "handle.hpp"
#include "renderer/renderer.hpp"

class Renderable_Component : public Base_Component {
private:
    Handle<renderer::Renderer> handle;

public:
    Renderable_Component(Game_Object&);
    virtual ~Renderable_Component();

    void set_material(Handle<Material>);
    void set_mesh(Handle<Mesh>);
    void set_transform(Matrix4 const&);
};

#endif // !ENGINE_COMPONENTS_RENDERABLE_COMPONENT_HPP_INCLUDE