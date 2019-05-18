#ifndef ENGINE_RENDERER_RENDERER_HPP_INCLUDE
#define ENGINE_RENDERER_RENDERER_HPP_INCLUDE

#include "components/camera.hpp"
#include "components/transform.hpp"
#include "shader.hpp"
#include <cstdint>

class Camera;
class Line_Component;
class Static_Mesh_Component;
class Matrix4;
class Transform;
class Mesh;

class Framebuffer;

class Renderer {
public:
    Renderer(uint32_t width, uint32_t height);
    ~Renderer();

    void resize(uint32_t width, uint32_t height);
    void render_frame(Camera, Transform camera_transform, uint32_t viewport_width, uint32_t viewport_height);
    uint32_t render_frame_as_texture(Camera, Transform camera_transform, uint32_t viewport_width, uint32_t viewport_height);
    void load_shader_light_properties();
    void set_gamma_value(float);

private:
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

    void build_framebuffers(uint32_t width, uint32_t height);
    void delete_framebuffers();

private:
    Framebuffer* framebuffer_multisampled;
    Framebuffer* framebuffer;
    Framebuffer* light_depth_buffer;

    Framebuffer* postprocess_front_buffer;
    Framebuffer* postprocess_back_buffer;

public:
    Shader default_shader;
    // Postprocessing shaders
    // TODO move to postprocessing
    Shader gamma_correction_shader;
    Shader passthrough_quad_shader;
    Shader tangents;
    Shader deferred_shading_shader;

public:
    float gamma_correction_value = 2.2f;
    uint32_t shadow_width = 1024;
    uint32_t shadow_height = 1024;
};

void bind_mesh_textures(Mesh const&, Shader&);
void render_mesh(Mesh const& mesh);

#endif // !ENGINE_RENDERER_RENDERER_HPP_INCLUDE
