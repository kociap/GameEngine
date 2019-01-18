#ifndef ENGINE_RENDERER_RENDERER_HPP_INCLUDE
#define ENGINE_RENDERER_RENDERER_HPP_INCLUDE

#include "handle.hpp"
#include "material.hpp"
#include "math/matrix4.hpp"
#include "mesh.hpp"
#include "object_manager.hpp"
#include <cstdint>

namespace renderer {
    class Renderer {
    public:
        void update();
        Handle<Renderer> register_component();
        void unregister_component(Handle<Renderer>);
        void set_material(Handle<Renderer>, Handle<Material>);
        void set_mesh(Handle<Renderer>, Handle<Mesh>);
        void set_local_to_world_transform(Handle<Renderer>, Matrix4 const&);
    };
} // namespace renderer

#endif // !ENGINE_RENDERER_RENDERER_HPP_INCLUDE
