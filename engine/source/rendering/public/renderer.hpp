#ifndef ENGINE_RENDERER_RENDERER_HPP_INCLUDE
#define ENGINE_RENDERER_RENDERER_HPP_INCLUDE

#include <components/camera.hpp>
#include <components/transform.hpp>
#include <cstdint>
#include <shader.hpp>

namespace anton_engine {
    class Camera;
    class Line_Component;
    class Static_Mesh_Component;
    class Matrix4;
    class Transform;
    class Mesh;

    class Framebuffer;
} // namespace anton_engine

namespace anton_engine::rendering {
    class Renderer {
    public:
        Renderer(int32_t width, int32_t height);
        ~Renderer();

        void resize(int32_t width, int32_t height);
        void render_frame(Matrix4 view_mat, Matrix4 proj_mat, Transform camera_transform, int32_t viewport_width, int32_t viewport_height);
        uint32_t render_frame_as_texture(Matrix4 view_mat, Matrix4 proj_mat, Transform camera_transform, int32_t viewport_width, int32_t viewport_height);
        void set_gamma_value(float);
        void swap_postprocess_buffers();

    private:
        void render_scene(Transform camera_transform, Matrix4 view_transform, Matrix4 projection_transform);

        void build_framebuffers(int32_t width, int32_t height);
        void delete_framebuffers();

    public:
        Framebuffer* framebuffer;
        Framebuffer* postprocess_front_buffer;
        Framebuffer* postprocess_back_buffer;

        Shader default_shader;
        // Postprocessing shaders
        // TODO move to postprocessing
        Shader gamma_correction_shader;
        Shader passthrough_quad_shader;
        Shader outline_mix_shader;
    };

    void setup_rendering();
    void update_dynamic_lights();
    void bind_mesh_vao();

    // Render a quad taking up the whole viewport
    // Intended for rendering textures to the screen or applying postprocessing effects
    // Rebinds element buffer and vertex buffer with binding index 0
    void render_texture_quad();
} // namespace anton_engine::rendering

#endif // !ENGINE_RENDERER_RENDERER_HPP_INCLUDE
