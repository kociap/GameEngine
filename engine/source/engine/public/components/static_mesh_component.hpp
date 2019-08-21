#ifndef ENGINE_COMPONENTS_STATIC_MESH_COMPONENT_HPP_INCLUDE
#define ENGINE_COMPONENTS_STATIC_MESH_COMPONENT_HPP_INCLUDE

#include <class_macros.hpp>
#include <handle.hpp>
#include <serialization/serialization.hpp>

class Mesh;
class Shader;
class Material;

class COMPONENT Static_Mesh_Component {
public:
    Handle<Mesh> mesh_handle;
    Handle<Shader> shader_handle;
    Handle<Material> material_handle;
};

DEFAULT_SERIALIZABLE(Static_Mesh_Component);
DEFAULT_DESERIALIZABLE(Static_Mesh_Component);

#endif // !ENGINE_COMPONENTS_STATIC_MESH_COMPONENT_HPP_INCLUDE
