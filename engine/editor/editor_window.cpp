#include <editor_window.hpp>
#include <ui_editor_window.h>

#include <components/camera.hpp>
#include <components/transform.hpp>
#include <ecs/ecs.hpp>
#include <editor.hpp>
#include <gizmo_internal.hpp>
#include <opengl.hpp>
#include <viewport.hpp>
#include <viewport_camera.hpp>

#include <stdexcept>

#include <QDockWidget>
#include <QOffscreenSurface>
#include <QOpenGLContext>
#include <QSurfaceFormat>

constexpr uint32_t max_viewports = 4;

Editor_Window::Editor_Window(QWidget* parent): QMainWindow(parent), viewports(max_viewports, nullptr), viewport_docks(max_viewports, nullptr) {
    context = new QOpenGLContext;
    context->setFormat(QSurfaceFormat::defaultFormat());
    if (!context->create()) {
        throw std::runtime_error("Could not create OpenGL context");
    }

    surface = new QOffscreenSurface;
    surface->setFormat(QSurfaceFormat::defaultFormat());
    surface->create();

    context->makeCurrent(surface);
    opengl::load_functions();
    opengl::load_constants();
    gizmo::init();

    ui = new Ui::Editor_Window;
    ui->setupUi(this);

    viewports[0] = new Viewport(context);
    viewport_docks[0] = new QDockWidget;
    viewport_docks[0]->setWidget(viewports[0]);
    addDockWidget(Qt::TopDockWidgetArea, viewport_docks[0]);
}

Editor_Window::~Editor_Window() {
    // We need a context to destroy opengl resources
    context->makeCurrent(surface);
    for (auto ptr: viewports) {
        delete ptr;
    }

    for (auto ptr: viewport_docks) {
        delete ptr;
    }

    gizmo::terminate();
    delete context;
    delete surface;
    delete ui;
}

void Editor_Window::update() {
    auto viewport_camera_view = Editor::get_ecs().access<Viewport_Camera, Camera, Transform>();
    for (Entity const entity: viewport_camera_view) {
        auto [viewport_camera, camera, transform] = viewport_camera_view.get<Viewport_Camera, Camera, Transform>(entity);
        Viewport_Camera::update(viewport_camera, transform);
        auto viewport = viewports[viewport_camera.viewport_index];
        if (viewport) {
            Matrix4 const view_mat = get_camera_view_matrix(transform);
            Matrix4 const projection_mat = get_camera_projection_matrix(camera, viewport->width(), viewport->height());
            Matrix4 const inv_projection_mat = math::inverse(projection_mat);
            Matrix4 const inv_view_mat = math::inverse(view_mat);
            viewport->process_actions(view_mat, projection_mat, inv_projection_mat, inv_view_mat, transform, shared_state.selected_entities);
        }
    }
}

void Editor_Window::render() {
    GE_log("Render");
    auto viewport_camera_view = Editor::get_ecs().access<Viewport_Camera, Camera, Transform>();
    for (Entity const entity: viewport_camera_view) {
        auto [viewport_camera, camera, transform] = viewport_camera_view.get<Viewport_Camera, Camera, Transform>(entity);
        Viewport_Camera::update(viewport_camera, transform);
        auto viewport = viewports[viewport_camera.viewport_index];
        if (viewport) {
            Matrix4 const view_mat = get_camera_view_matrix(transform);
            Matrix4 const projection_mat = get_camera_projection_matrix(camera, viewport->width(), viewport->height());
            viewport->render(view_mat, projection_mat, transform, shared_state.selected_entities);
        }
    }
}
