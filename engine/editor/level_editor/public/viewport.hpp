#ifndef EDITOR_LEVEL_EDITOR_VIEWPORT_HPP_INCLUDE
#define EDITOR_LEVEL_EDITOR_VIEWPORT_HPP_INCLUDE

#include <components/transform.hpp>
#include <containers/vector.hpp>
#include <ecs/entity.hpp>
#include <math/matrix4.hpp>
#include <math/vector3.hpp>

#include <QWidget>

class Framebuffer;
class QOpenGLContext;
class QPaintEngine;
class QMouseEvent;
class QCloseEvent;
namespace rendering {
    class Renderer;
}

class Viewport: public QWidget {
    Q_OBJECT

public:
    explicit Viewport(int32_t viewport_index, QOpenGLContext* context);
    Viewport(int32_t viewport_index, QWidget* parent, QOpenGLContext* context);
    ~Viewport();

    void process_actions(Matrix4 view_mat, Matrix4 projection_mat, Matrix4 inv_view_mat, Matrix4 inv_projection_mat, Transform camera_transform,
                         containers::Vector<Entity>& selected_entities);
    void render(Matrix4 view_mat, Matrix4 projection_mat, Transform camera_transform, containers::Vector<Entity> const&);
    void resize(int w, int h);
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
    rendering::Renderer* renderer = nullptr;
    QOpenGLContext* context;

    Entity viewport_entity = null_entity;
    int32_t index;

    // No idea what most of that means
    Transform cached_gizmo_transform;
    Vector3 gizmo_mouse_grab_point;
    Vector3 gizmo_grabbed_axis;
    Vector3 gizmo_plane_normal;
    float gizmo_plane_distance;
    bool gizmo_grabbed = false;

    // Mouse movement
    int32_t lock_pos_x = 0;
    int32_t lock_pos_y = 0;
    bool cursor_locked = false;
};

#endif // !EDITOR_LEVEL_EDITOR_VIEWPORT_HPP_INCLUDE
