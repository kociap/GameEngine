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

namespace anton_engine {
    class Framebuffer;
    namespace rendering {
        class Renderer;
    }

    namespace imgui {
        class Context;
    }

    class Viewport {
    public:
        explicit Viewport(int32_t viewport_index);
        ~Viewport();

        void process_actions(Matrix4 view_mat, Matrix4 projection_mat, Matrix4 inv_view_mat, Matrix4 inv_projection_mat, Transform camera_transform,
                             atl::Vector<Entity>& selected_entities);
        void render(Matrix4 view_mat, Matrix4 inv_view_mat, Matrix4 projection_mat, Matrix4 inv_projection_mat, Camera, Transform camera_transform,
                    atl::Slice<Entity const>);
        void resize(int32_t w, int32_t h);

    private:
        Framebuffer* framebuffer = nullptr;
        Framebuffer* multisampled_framebuffer = nullptr;
        Framebuffer* deferred_framebuffer = nullptr;
        imgui::Context* imgui_context;

        Entity viewport_entity = null_entity;
        int32_t index;
    };
} // namespace anton_engine

#endif // !EDITOR_LEVEL_EDITOR_VIEWPORT_HPP_INCLUDE
