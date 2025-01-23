#pragma once

#include <anton/array.hpp>
#include <anton/math/mat4.hpp>
#include <anton/math/vec3.hpp>
#include <anton/slice.hpp>
#include <core/diagnostic_macros.hpp>
#include <core/types.hpp>
#include <engine/components/camera.hpp>
#include <engine/components/transform.hpp>
#include <engine/ecs/entity.hpp>
#include <level_editor/gizmo_context.hpp>

namespace anton_engine {
  class Framebuffer;
  namespace imgui {
    class Context;
  }

  class Viewport {
  public:
    explicit Viewport(i32 viewport_index, i32 width, i32 height,
                      imgui::Context*);
    ~Viewport();

    void process_actions(Mat4 view_mat, Mat4 inv_view_mat, Mat4 proj_mat,
                         Mat4 inv_proj_mat, Transform camera_transform,
                         anton::Array<Entity>& selected_entities);
    void render(Mat4 view_mat, Mat4 inv_view_mat, Mat4 proj_mat,
                Mat4 inv_proj_mat, Camera camera, Transform camera_transform,
                anton::Slice<Entity const> selected_entities);
    void resize_framebuffers(i32 width, i32 height);
    Vec2 get_size() const;
    bool is_active() const;

  private:
    Framebuffer* framebuffer = nullptr;
    Framebuffer* multisampled_framebuffer = nullptr;
    Framebuffer* deferred_framebuffer = nullptr;
    Framebuffer* front_framebuffer = nullptr;
    Framebuffer* back_framebuffer = nullptr;
    imgui::Context* ctx;

    Entity viewport_entity = null_entity;
    i32 index;
  };
} // namespace anton_engine
