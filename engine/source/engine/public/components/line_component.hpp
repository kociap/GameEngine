#ifndef ENGINE_COMPONENTS_LINE_COMPONENT_HPP_INCLUDE
#define ENGINE_COMPONENTS_LINE_COMPONENT_HPP_INCLUDE

#include "color.hpp"
#include "handle.hpp"

class Shader;
class Mesh;

class Line_Component {
public:
    Color color = Color::white;
    float thickness = 1;

    Handle<Mesh> mesh_handle;
    Handle<Shader> shader_handle;
};

#endif // !ENGINE_COMPONENTS_LINE_COMPONENT_HPP_INCLUDE