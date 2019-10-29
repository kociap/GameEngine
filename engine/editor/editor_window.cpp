#include <editor_window.hpp>

#include <anton_stl/string.hpp>
#include <builtin_shaders.hpp>
#include <components/camera.hpp>
#include <components/transform.hpp>
#include <ecs/ecs.hpp>
#include <editor_preferences.hpp>
#include <input/input.hpp>
#include <logging.hpp>
#include <opengl.hpp>
#include <renderer.hpp>
#include <viewport_camera.hpp>

#include <dock_widget.hpp>
#include <editor.hpp>
#include <log_viewer.hpp>
#include <outliner.hpp>
#include <user_input_filter.hpp>
#include <viewport.hpp>

#include <stdexcept>

ANTON_DISABLE_WARNINGS();
#include <QMouseEvent>
#include <QOffscreenSurface>
#include <QOpenGLContext>
#include <QSurfaceFormat>
#include <QWindow>
ANTON_RESTORE_WARNINGS();

namespace anton_engine {
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
        opengl::load();
        rendering::setup_rendering();
        load_builtin_shaders();
        load_default_editor_preferences();

        input_filter = new User_Input_Filter(0, 0);
        installEventFilter(input_filter);
    }

    Editor_Window::~Editor_Window() {
        // TODO: Exception when delete is called
        // delete outliner_dock;
        // delete outliner;

        // We need a context to destroy opengl resources
        context->makeCurrent(surface);
        for (auto ptr: viewports) {
            delete ptr;
        }

        for (auto ptr: viewport_docks) {
            delete ptr;
        }

        unload_builtin_shaders();
        delete context;
        delete surface;
        delete input_filter;
    }

    void Editor_Window::setup_interface() {
        //auto viewport_closed = [this](int32_t const index) {
        //    removeDockWidget(viewport_docks[index]);
        //    delete viewport_docks[index];
        //    viewport_docks[index] = nullptr;
        //    delete viewports[index];
        //    viewports[index] = nullptr;
        //};

        log_viewer_dock = new Dock_Widget;
        log_viewer_dock->setObjectName("Log Viewer Dock");
        log_viewer = new Log_Viewer;
        log_viewer_dock->setWidget(log_viewer);
        addDockWidget(Qt::BottomDockWidgetArea, log_viewer_dock);

        outliner = new Outliner;
        outliner_dock = new Dock_Widget;
        outliner_dock->setObjectName("Outliner Dock");
        outliner_dock->setWidget(outliner);
        addDockWidget(Qt::BottomDockWidgetArea, outliner_dock);

        viewports[0] = new Viewport(0, context);
        viewport_docks[0] = new Dock_Widget;
        viewport_docks[0]->setWidget(viewports[0]);
        addDockWidget(Qt::TopDockWidgetArea, viewport_docks[0]);
        connect(viewports[0], &Viewport::made_active, [this](int32_t const index) {
            shared_state.active_editor = index;
            anton_engine::log_message(anton_engine::Log_Message_Severity::info, u8"Changed active viewport to " + anton_stl::to_string(index));
        });
        //connect(viewport_docks[0], &Dock_Widget::window_closed, viewports[0], &Viewport::close);
        //connect(viewports[0], &Viewport::window_closed, viewport_closed);

        // viewports[1] = new Viewport(1, context);
        // viewport_docks[1] = new Dock_Widget;
        // viewport_docks[1]->setWidget(viewports[1]);
        // addDockWidget(Qt::TopDockWidgetArea, viewport_docks[1]);
        // connect(viewports[1], &Viewport::made_active, [this](int32_t const index) {
        //     shared_state.active_editor = index;
        //     anton_engine::log_message(anton_engine::Log_Message_Severity::info, u8"Changed active viewport to " + anton_stl::to_string(index));
        // });
        //connect(viewport_docks[1], &Dock_Widget::window_closed, viewports[1], &Viewport::close);
        //connect(viewports[1], &Viewport::window_closed, viewport_closed);
    }

    void Editor_Window::update() {
        if (auto const state = Input::get_key_state(Key::y); state.down && state.up_down_transitioned) {
            reload_builtin_shaders();
        }

        outliner->update();

        auto const state = Input::get_key_state(Key::u);
        if (state.up_down_transitioned && !state.down) {
            if (!viewports[shared_state.active_editor]->is_cursor_locked()) {
                viewports[shared_state.active_editor]->lock_cursor_at(QCursor::pos().x(), QCursor::pos().y());
            } else {
                viewports[shared_state.active_editor]->unlock_cursor();
            }
        }

        auto viewport_camera_view = Editor::get_ecs().view<Viewport_Camera, Camera, Transform>();
        for (Entity const entity: viewport_camera_view) {
            auto [viewport_camera, camera, transform] = viewport_camera_view.get<Viewport_Camera, Camera, Transform>(entity);
            auto viewport = viewports[viewport_camera.viewport_index];
            if (viewport && shared_state.active_editor == viewport_camera.viewport_index) {
                Viewport_Camera::update(viewport_camera, transform);
                Matrix4 const view_mat = get_camera_view_matrix(transform);
                Matrix4 const projection_mat = get_camera_projection_matrix(camera, viewport->width(), viewport->height());
                Matrix4 const inv_projection_mat = math::inverse(projection_mat);
                Matrix4 const inv_view_mat = math::inverse(view_mat);
                viewport->process_actions(view_mat, projection_mat, inv_view_mat, inv_projection_mat, transform, shared_state.selected_entities);
            }
        }
    }

    void Editor_Window::render() {
        static bool made_current = false;
        if (!made_current) {
            context->makeCurrent(viewports[0]->windowHandle());
        }

        rendering::update_dynamic_lights();

        auto viewport_camera_view = Editor::get_ecs().view<Viewport_Camera, Camera, Transform>();
        for (Entity const entity: viewport_camera_view) {
            auto [viewport_camera, camera, transform] = viewport_camera_view.get<Viewport_Camera, Camera, Transform>(entity);
            auto viewport = viewports[viewport_camera.viewport_index];
            if (viewport) {
                Matrix4 const view_mat = get_camera_view_matrix(transform);
                Matrix4 const projection_mat = get_camera_projection_matrix(camera, viewport->width(), viewport->height());
                viewport->render(view_mat, projection_mat, camera, transform, shared_state.selected_entities);
            }
        }
    }
} // namespace anton_engine
