#ifndef ENGINE_COMPONENTS_STATIC_MESH_COMPONENT_HPP_INCLUDE
#define ENGINE_COMPONENTS_STATIC_MESH_COMPONENT_HPP_INCLUDE

#include "handle.hpp"

class Mesh;
class Shader;

class Static_Mesh_Component {
public:
    Handle<Mesh> mesh_handle;
    Handle<Shader> shader_handle;
};

#endif // !ENGINE_COMPONENTS_STATIC_MESH_COMPONENT_HPP_INCLUDE