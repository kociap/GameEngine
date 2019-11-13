#ifndef EDITOR_LEVEL_EDITOR_VIEWPORT_HPP_INCLUDE
#define EDITOR_LEVEL_EDITOR_VIEWPORT_HPP_INCLUDE

#include <anton_stl/slice.hpp>
#include <anton_stl/vector.hpp>
#include <components/camera.hpp>
#include <components/transform.hpp>
#include <diagnostic_macros.hpp>
#include <ecs/entity.hpp>
#include <gizmo_context.hpp>
#include <math/matrix4.hpp>
#include <math/vector3.hpp>

ANTON_DISABLE_WARNINGS();
#include <QWidget>
ANTON_RESTORE_WARNINGS();

class QOpenGLContext;
class QPaintEngine;
class QMouseEvent;
class QCloseEvent;

namespace anton_engine {
    class Framebuffer;
    namespace rendering {
        class Renderer;
    }

    class Viewport: public QWidget {
        Q_OBJECT

    public:
        explicit Viewport(int32_t viewport_index, QOpenGLContext* context);
        Viewport(int32_t viewport_index, QWidget* parent, QOpenGLContext* context);
        ~Viewport() override;

        void process_actions(Matrix4 view_mat, Matrix4 projection_mat, Matrix4 inv_view_mat, Matrix4 inv_projection_mat, Transform camera_transform,
                             anton_stl::Vector<Entity>& selected_entities);
        void render(Matrix4 view_mat, Matrix4 inv_view_mat, Matrix4 projection_mat, Matrix4 inv_projection_mat, Camera, Transform camera_transform,
                    anton_stl::Slice<Entity const>);
        void resize(int32_t w, int32_t h);
        bool is_cursor_locked() const;
        void lock_cursor_at(int32_t x, int32_t y);
        void unlock_cursor();

        QPaintEngine* paintEngine() const override;

    protected:
        void closeEvent(QCloseEvent*) override;
        void resizeEvent(QResizeEvent*) override;
        void mouseMoveEvent(QMouseEvent*) override;
        void mousePressEvent(QMouseEvent*) override;

    Q_SIGNALS:
        void window_closed(int32_t index);
        void made_active(int32_t index);

    private:
        Framebuffer* framebuffer = nullptr;
        Framebuffer* multisampled_framebuffer = nullptr;
        Framebuffer* deferred_framebuffer = nullptr;
        QOpenGLContext* context;

        Entity viewport_entity = null_entity;
        int32_t index;

        // Mouse movement
        int32_t lock_pos_x = 0;
        int32_t lock_pos_y = 0;
        bool cursor_locked = false;
    };
} // namespace anton_engine

#endif // !EDITOR_LEVEL_EDITOR_VIEWPORT_HPP_INCLUDE
