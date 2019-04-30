#ifndef ENGINE_RENDERER_RENDERER_HPP_INCLUDE
#define ENGINE_RENDERER_RENDERER_HPP_INCLUDE

#include "shader.hpp"
#include <cstdint>

class Camera;
class Line_Component;
class Static_Mesh_Component;
class Matrix4;
class Transform;
class Mesh;

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
        void render_mesh(Mesh& mesh, Shader& shader);
        void render_mesh_instanced(Mesh& mesh, Shader& shader, uint32_t count);
        void render_object(Static_Mesh_Component const&, Shader&);
        void render_object(Line_Component const&, Shader&);
        void render_shadow_map(Matrix4 const& view_transform, Matrix4 const& projection_transform);
        void render_scene(Transform const& camera_transform, Matrix4 const& view_transform, Matrix4 const& projection_transform,
                          Matrix4 const& light_space_transform);
        void render_with_shader(Shader& shader, Transform const& camera_transform, Matrix4 const& view_transform, Matrix4 const& projection_transform);
        void update_dynamic_lights();
        void setup_opengl();
        void swap_postprocess_buffers();

    private:
        Framebuffer* framebuffer_multisampled;
        Framebuffer* framebuffer;
        Framebuffer* light_depth_buffer;

        Framebuffer* postprocess_front_buffer;
        Framebuffer* postprocess_back_buffer;

    public:
        // Postprocessing shaders
        // TODO move to postprocessing
        Shader gamma_correction_shader;
        Shader quad_shader;
        Shader tangents;
        Shader deferred_shading_shader;

    public:
        float gamma_correction_value = 2.2f;
        uint32_t shadow_width = 1024;
        uint32_t shadow_height = 1024;
        bool output_shadow_map = false;
    };
} // namespace renderer

#endif // !ENGINE_RENDERER_RENDERER_HPP_INCLUDE
