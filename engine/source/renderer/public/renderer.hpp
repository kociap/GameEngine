#ifndef ENGINE_RENDERER_RENDERER_HPP_INCLUDE
#define ENGINE_RENDERER_RENDERER_HPP_INCLUDE

#include "shader.hpp"
#include <cstdint>

class Camera;
class Line_Component;
class Static_Mesh_Component;
class Matrix4;
class Transform;

namespace renderer {
    class Framebuffer;

    class Renderer {
    public:
        Renderer();
        ~Renderer();

        void render_frame();
        void load_shader_light_properties();
        void set_gamma_value(float);

    private:
        void render_object(Static_Mesh_Component const&, Shader&);
        void render_object(Line_Component const&, Shader&);
        void render_scene(Transform const& camera_transform, Matrix4 const& view_transform, Matrix4 const& projection_transform);
        Camera& find_active_camera();
        void setup_opengl();

    private:
        Framebuffer* framebuffer_multisampled;
        Framebuffer* framebuffer;
        Framebuffer* light_depth_buffer;

        // Postprocessing shaders
        // TODO move to postprocessing
        Shader gamma_correction_shader;
        Shader quad_shader;

    public:
        float gamma_correction_value = 2.2f;
        uint32_t shadow_width = 1024;
        uint32_t shadow_height = 1024;
    };
} // namespace renderer

#endif // !ENGINE_RENDERER_RENDERER_HPP_INCLUDE
