#ifndef ENGINE_RENDERER_RENDERER_HPP_INCLUDE
#define ENGINE_RENDERER_RENDERER_HPP_INCLUDE

#include <components/camera.hpp>
#include <components/transform.hpp>
#include <cstdint>
#include <shader.hpp>

class Camera;
class Line_Component;
class Static_Mesh_Component;
class Matrix4;
class Transform;
class Mesh;

class Framebuffer;

namespace rendering {
    // TODO this thing has to go
    class Renderer {
    public:
        Renderer(int32_t width, int32_t height);
        ~Renderer();

        void resize(int32_t width, int32_t height);
        void render_frame(Matrix4 view_mat, Matrix4 proj_mat, Transform camera_transform, int32_t viewport_width, int32_t viewport_height);
        uint32_t render_frame_as_texture(Matrix4 view_mat, Matrix4 proj_mat, Transform camera_transform, int32_t viewport_width, int32_t viewport_height);
        void load_shader_light_properties();
        void set_gamma_value(float);
        void swap_postprocess_buffers();

    private:
        void render_shadow_map(Matrix4 const& view_transform, Matrix4 const& projection_transform);
        void render_scene(Transform camera_transform, Matrix4 view_transform, Matrix4 projection_transform);
        void render_with_shader(Shader& shader, Transform const& camera_transform, Matrix4 const& view_transform, Matrix4 const& projection_transform);
        void update_dynamic_lights();
        void setup_opengl();

        void build_framebuffers(int32_t width, int32_t height);
        void delete_framebuffers();

    private:
        Framebuffer* framebuffer_multisampled;

    public:
        Framebuffer* framebuffer;
        Framebuffer* postprocess_front_buffer;
        Framebuffer* postprocess_back_buffer;
        // Standard vao used for rendering meshes with position, normals, texture coords, tangent and bitangent
        // Uses binding index 0
        uint32_t mesh_vao;

        Shader default_shader;
        // Postprocessing shaders
        // TODO move to postprocessing
        Shader gamma_correction_shader;
        Shader passthrough_quad_shader;
        Shader tangents;
        Shader deferred_shading_shader;
        Shader single_color_shader;
        Shader outline_mix_shader;

        uint32_t shadow_width = 1024;
        uint32_t shadow_height = 1024;
    };

    // Render a quad taking up the whole viewport
    // Intended for rendering textures to the screen or applying postprocessing effects
    // Rebinds element buffer and vertex buffer with binding index 0
    void render_texture_quad();
} // namespace rendering
#endif // !ENGINE_RENDERER_RENDERER_HPP_INCLUDE
