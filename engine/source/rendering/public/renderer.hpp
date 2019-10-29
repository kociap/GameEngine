#ifndef ENGINE_RENDERER_RENDERER_HPP_INCLUDE
#define ENGINE_RENDERER_RENDERER_HPP_INCLUDE

#include <anton_int.hpp>
#include <anton_stl/slice.hpp>
#include <components/camera.hpp>
#include <components/transform.hpp>
#include <cstdint>
#include <ecs/ecs.hpp>
#include <material.hpp>
#include <math/matrix4.hpp>
#include <math/vector2.hpp>
#include <mesh.hpp>
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
        void render_frame(Matrix4 view_mat, Matrix4 proj_mat, Transform camera_transform, Vector2 viewport_size);
        void swap_postprocess_buffers();

    private:
        void build_framebuffers(int32_t width, int32_t height);
        void delete_framebuffers();

    public:
        Framebuffer* framebuffer;
        Framebuffer* postprocess_front_buffer;
        Framebuffer* postprocess_back_buffer;
    };

    struct Draw_Arrays_Command {
        u32 count;
        u32 instance_count;
        u32 first;
        u32 base_instance;
    };

    struct Draw_Elements_Command {
        u32 count;
        u32 instance_count;
        u32 first_index;
        u32 base_vertex;
        u32 base_instance;
    };

    struct Draw_Persistent_Geometry_Command {
        u32 handle;
        u32 instance_count;
        u32 base_instance;
    };

    template <typename T>
    struct Mapped_Buffer {
        T* data;
        i64 size;
    };

    using Vertex_Buffer = Mapped_Buffer<void>;
    using Matrix_Buffer = Mapped_Buffer<Matrix4>;
    using Material_Buffer = Mapped_Buffer<Material>;
    using Element_Buffer = Mapped_Buffer<u32>;
    using Draw_ID_Buffer = Mapped_Buffer<u32>;

    void setup_rendering();
    void update_dynamic_lights();
    void bind_mesh_vao();
    void bind_vertex_buffers();
    [[nodiscard]] Vertex_Buffer get_vertex_buffer();
    [[nodiscard]] Draw_ID_Buffer get_draw_id_buffer();
    [[nodiscard]] Matrix_Buffer get_matrix_buffer();
    [[nodiscard]] Material_Buffer get_material_buffer();
    [[nodiscard]] Element_Buffer get_element_buffer();

    // Returns: Handle to the persistent geometry.
    u64 write_persistent_geometry(anton_stl::Slice<Vertex const>, anton_stl::Slice<u32 const>);

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
    void add_draw_command(Draw_Persistent_Geometry_Command);
    void commit_draw();

    // objects - snapshot of ecs containing Static_Mesh_Components and Transforms
    void render_scene(ECS objects, Transform camera_transform, Matrix4 view, Matrix4 projection);

    // Render a quad taking up the whole viewport
    // Intended for rendering textures to the screen or applying postprocessing effects
    // Rebinds element buffer and vertex buffer with binding index 0
    void render_texture_quad();
} // namespace anton_engine::rendering

#endif // !ENGINE_RENDERER_RENDERER_HPP_INCLUDE
