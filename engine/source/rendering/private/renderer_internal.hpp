#ifndef RENDERER_RENDERER_INTERNAL_HPP_INCLUDE
#define RENDERER_RENDERER_INTERNAL_HPP_INCLUDE

#include <material.hpp>

namespace anton_engine {
    class Mesh;
    class Shader;
    class Line_Component;
    class Static_Mesh_Component;
} // namespace anton_engine

namespace anton_engine::rendering {
    void bind_material_properties(Material, Shader&);
    void render_mesh(Mesh const& mesh);
    void render_object(Static_Mesh_Component const&, Shader&);
    void render_object(Line_Component const&, Shader&);
} // namespace anton_engine::rendering

#endif // !RENDERER_RENDERER_INTERNAL_HPP_INCLUDE
