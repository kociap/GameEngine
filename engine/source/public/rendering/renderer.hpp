#pragma once

#include <anton/math/mat4.hpp>
#include <anton/math/vec2.hpp>
#include <anton/slice.hpp>
#include <core/types.hpp>
#include <engine/components/camera.hpp>
#include <engine/components/transform.hpp>
#include <engine/ecs/ecs.hpp>
#include <engine/material.hpp>
#include <engine/mesh.hpp>
#include <rendering/texture_format.hpp>
#include <shaders/shader.hpp>

namespace anton_engine {
  class Camera;
  class Line_Component;
  class Static_Mesh_Component;
  class Transform;
  class Mesh;
  class Framebuffer;
} // namespace anton_engine

namespace anton_engine::rendering {
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

  template<typename T>
  struct Buffer {
    T* buffer;
    T* head;
    i64 size;
  };

  void setup_rendering();
  void bind_persistent_geometry_buffers();
  void bind_transient_geometry_buffers();
  void bind_mesh_vao();
  void bind_buffers();
  void update_dynamic_lights();

  // Write geometry to gpu buffers. The geometry will eventually be overwritten.
  [[nodiscard]] Draw_Elements_Command write_geometry(anton::Slice<Vertex const>,
                                                     anton::Slice<u32 const>);

  // Write matrices and materials to gpu buffers. Data will eventually be overwritten.
  // Returns draw_id offset to be used as base_instance in draw commands.
  [[nodiscard]] u32 write_matrices_and_materials(anton::Slice<Mat4 const>,
                                                 anton::Slice<Material const>);

  // Write geometry that will persist across multiple frames. Geometry will not be overwritten.
  // Returns: Handle to the persistent geometry.
  [[nodiscard]] u64 write_persistent_geometry(anton::Slice<Vertex const>,
                                              anton::Slice<u32 const>);

  // Loads base texture and generates mipmaps (since we don't have pregenerated mipmaps yet).
  // pixels is a pointer to an array of pointers to the pixel data.
  // handles (out) array of handles to the textures. Must be at least texture_count big.
  // handle <internal texture index (u32), layer (f32)>
  void load_textures_generate_mipmaps(Texture_Format, i32 texture_count,
                                      void const* const* pixels,
                                      Texture* handles);

  // handle <internal texture index (u32), layer (f32)>
  // The handle is translated to gl texture handle and then bound.
  // Since textures are stored as array textures, textures with same index do not have to
  //   be bound multiple times.
  void bind_texture(u32 unit, Texture handle);

  // TODO: Texture unloading.
  // void unload_texture(u64 handle);
  // void unload_textures(i32 handle_count, u64 const* handles);

  void add_draw_command(Draw_Elements_Command);
  void add_draw_command(Draw_Persistent_Geometry_Command);
  void commit_draw();

  // objects - snapshot of ecs containing Static_Mesh_Components and Transforms
  void render_scene(ECS objects, Transform camera_transform, Mat4 view,
                    Mat4 projection);

  // Render a quad taking up the whole viewport
  // Intended for rendering textures to the screen or applying postprocessing effects
  // Rebinds element buffer and vertex buffer with binding index 0
  void render_texture_quad();

  // Ugly thing
  class Renderer {
  public:
    Renderer(i32 width, i32 height);
    ~Renderer();

    void resize(i32 width, i32 height);
    void render_frame(Mat4 view_mat, Mat4 proj_mat, Transform camera_transform,
                      Vec2 viewport_size);
    void swap_postprocess_buffers();

  private:
    void build_framebuffers(i32 width, i32 height);
    void delete_framebuffers();

  public:
    Framebuffer* framebuffer;
    Framebuffer* postprocess_front_buffer;
    Framebuffer* postprocess_back_buffer;
  };
} // namespace anton_engine::rendering
