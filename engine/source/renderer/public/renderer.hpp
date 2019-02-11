#ifndef ENGINE_RENDERER_RENDERER_HPP_INCLUDE
#define ENGINE_RENDERER_RENDERER_HPP_INCLUDE

#include "components/renderable_component.hpp"
#include "containers/swapping_pool.hpp"
#include "framebuffer.hpp"
#include "handle.hpp"
#include "math/matrix4.hpp"
#include <cstdint>
#include "shader.hpp"

class Camera;

namespace renderer {
    class Renderer {
    public:
        Renderer();

        void register_component(Renderable_Component*);
        void unregister_component(Renderable_Component*);
        void render_frame();
        void load_shaders();

    private:
        void render_scene(Transform const& camera_transform, Matrix4 const& view_transform, Matrix4 const& projection_transform);
        void load_shaders_properties();
        Camera& find_active_camera();

    private:
        Swapping_Pool<Renderable_Component*> registered_components;
        Framebuffer* framebuffer_multisampled;
        Framebuffer* framebuffer;
        Framebuffer* light_depth_buffer;

        Shader shader;
        Shader light_shader;
        Shader gamma_correction_shader;
        Shader quad_shader;

    public:
        float gamma_correction_value = 2.2f;
    };
} // namespace renderer

#endif // !ENGINE_RENDERER_RENDERER_HPP_INCLUDE
