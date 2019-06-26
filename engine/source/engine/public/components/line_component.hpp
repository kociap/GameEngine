#ifndef ENGINE_COMPONENTS_LINE_COMPONENT_HPP_INCLUDE
#define ENGINE_COMPONENTS_LINE_COMPONENT_HPP_INCLUDE

#include <class_macros.hpp>
#include <color.hpp>
#include <handle.hpp>
#include <serialization.hpp>

class Shader;
class Mesh;
class Material;

class COMPONENT Line_Component {
public:
    Color color = Color::white;
    float thickness = 1;

    Handle<Mesh> mesh_handle;
    Handle<Shader> shader_handle;
    Handle<Material> material_handle;
};

template <>
struct serialization::use_default_deserialize<Line_Component>: std::true_type {};

#endif // !ENGINE_COMPONENTS_LINE_COMPONENT_HPP_INCLUDE