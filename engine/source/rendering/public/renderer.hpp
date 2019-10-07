#ifndef ENGINE_RENDERER_RENDERER_HPP_INCLUDE
#define ENGINE_RENDERER_RENDERER_HPP_INCLUDE

#include <components/camera.hpp>
#include <components/transform.hpp>
#include <cstdint>
#include <material.hpp>
#include <shader.hpp>
#include <texture_format.hpp>

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

    struct Draw_Arrays_Command {
        uint32_t count;
        uint32_t instance_count;
        uint32_t first;
        uint32_t base_instance;
    };

    struct Draw_Elements_Command {
        uint32_t count;
        uint32_t instance_count;
        uint32_t firts_index;
        uint32_t base_vertex;
        uint32_t base_instance;
    };

    struct Mapped_Buffer {
        void* data;
        int64_t size;
    };

    void setup_rendering();
    void update_dynamic_lights();
    void bind_mesh_vao();
    [[nodiscard]] Mapped_Buffer get_vertex_buffer();
    [[nodiscard]] Mapped_Buffer get_draw_id_buffer();
    [[nodiscard]] Mapped_Buffer get_matrix_buffer();
    [[nodiscard]] Mapped_Buffer get_material_buffer();
    [[nodiscard]] Mapped_Buffer get_element_buffer();

    // Loads base texture and generates mipmaps (since we don't have pregenerated mipmaps yet).
    // pixels is a pointer to an array of pointers to the pixel data.
    // handles (out) array of handles to the textures. Must be at least texture_count big.
    // handle <gl texture handle (u32), layer (f32)>
    void load_textures_generate_mipmaps(Texture_Format, int32_t texture_count, void const* const* pixels, Texture* handles);
    void bind_texture(uint32_t unit, Texture handle);
    // void unload_texture(uint64_t handle);
    // void unload_textures(int32_t handle_count, uint64_t const* handles);
    void add_draw_command(Draw_Arrays_Command);
    void add_draw_command(Draw_Elements_Command);
    void commit_draw();

    // Render a quad taking up the whole viewport
    // Intended for rendering textures to the screen or applying postprocessing effects
    // Rebinds element buffer and vertex buffer with binding index 0
    void render_texture_quad();
} // namespace anton_engine::rendering

#endif // !ENGINE_RENDERER_RENDERER_HPP_INCLUDE
