#ifndef EDITOR_LEVEL_EDITOR_VIEWPORT_HPP_INCLUDE
#define EDITOR_LEVEL_EDITOR_VIEWPORT_HPP_INCLUDE

#include <core/atl/slice.hpp>
#include <core/atl/vector.hpp>
#include <engine/components/camera.hpp>
#include <engine/components/transform.hpp>
#include <core/diagnostic_macros.hpp>
#include <engine/ecs/entity.hpp>
#include <level_editor/gizmo_context.hpp>
#include <core/math/matrix4.hpp>
#include <core/math/vector3.hpp>
#include <core/types.hpp>

namespace anton_engine {
    class Framebuffer;
    namespace imgui {
        class Context;
    }

    class Viewport {
    public:
        explicit Viewport(i32 viewport_index, i32 width, i32 height, imgui::Context*);
        ~Viewport();

        void process_actions(Matrix4 view_mat, Matrix4 inv_view_mat, Matrix4 proj_mat, Matrix4 inv_proj_mat,
                             Transform camera_transform, atl::Vector<Entity>& selected_entities);
        void render(Matrix4 view_mat, Matrix4 inv_view_mat, Matrix4 proj_mat, Matrix4 inv_proj_mat, 
                    Camera camera, Transform camera_transform, atl::Slice<Entity const> selected_entities);
        void resize_framebuffers(i32 width, i32 height);
        Vector2 get_size() const;
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

#endif // !EDITOR_LEVEL_EDITOR_VIEWPORT_HPP_INCLUDE
