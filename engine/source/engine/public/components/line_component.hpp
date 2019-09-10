#ifndef ENGINE_COMPONENTS_LINE_COMPONENT_HPP_INCLUDE
#define ENGINE_COMPONENTS_LINE_COMPONENT_HPP_INCLUDE

#include <class_macros.hpp>
#include <color.hpp>
#include <handle.hpp>
#include <serialization/serialization.hpp>

namespace anton_engine {
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
} // namespace anton_engine

ANTON_DEFAULT_SERIALIZABLE(anton_engine::Line_Component);

#endif // !ENGINE_COMPONENTS_LINE_COMPONENT_HPP_INCLUDE
