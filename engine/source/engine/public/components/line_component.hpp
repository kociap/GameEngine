#ifndef ENGINE_COMPONENTS_LINE_COMPONENT_HPP_INCLUDE
#define ENGINE_COMPONENTS_LINE_COMPONENT_HPP_INCLUDE

#include "color.hpp"
#include "handle.hpp"
#include "renderable_component.hpp"

class Shader;
class Mesh;

class Line_Component : public Renderable_Component {
public:
    Color color = Color::white;
    float thickness = 1;

    Line_Component(Entity const&);
    using Renderable_Component::Renderable_Component;

    Handle<Mesh> mesh_handle;
    Handle<Shader> shader_handle;
};

#endif // !ENGINE_COMPONENTS_LINE_COMPONENT_HPP_INCLUDE