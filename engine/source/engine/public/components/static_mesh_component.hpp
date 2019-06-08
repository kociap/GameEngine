#ifndef ENGINE_COMPONENTS_STATIC_MESH_COMPONENT_HPP_INCLUDE
#define ENGINE_COMPONENTS_STATIC_MESH_COMPONENT_HPP_INCLUDE

#include "handle.hpp"
#include "serialization.hpp"

class Mesh;
class Shader;
class Material;

class Static_Mesh_Component {
public:
    Handle<Mesh> mesh_handle;
    Handle<Shader> shader_handle;
    Handle<Material> material_handle;
};

template <>
struct serialization::use_default_deserialize<Static_Mesh_Component>: std::true_type {};

#endif // !ENGINE_COMPONENTS_STATIC_MESH_COMPONENT_HPP_INCLUDE