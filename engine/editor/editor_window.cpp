#include <editor_window.hpp>

#include <components/camera.hpp>
#include <components/transform.hpp>
#include <dock_widget.hpp>
#include <ecs/ecs.hpp>
#include <editor.hpp>
#include <gizmo_internal.hpp>
#include <input/input.hpp>
#include <opengl.hpp>
#include <outliner.hpp>
#include <user_input_filter.hpp>
#include <viewport.hpp>
#include <viewport_camera.hpp>

#include <stdexcept>

ANTON_DISABLE_WARNINGS()
#include <QMouseEvent>
#include <QOffscreenSurface>
#include <QOpenGLContext>
#include <QSurfaceFormat>
ANTON_RESTORE_WARNINGS()

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

    //auto viewport_closed = [this](int32_t const index) {
    //    removeDockWidget(viewport_docks[index]);
    //    delete viewport_docks[index];
    //    viewport_docks[index] = nullptr;
    //    delete viewports[index];
    //    viewports[index] = nullptr;
    //};

    viewports[0] = new Viewport(0, context);
    viewport_docks[0] = new Dock_Widget;
    viewport_docks[0]->setWidget(viewports[0]);
    addDockWidget(Qt::TopDockWidgetArea, viewport_docks[0]);
    connect(viewports[0], &Viewport::made_active, [this](int32_t const index) {
        shared_state.active_editor = index;
        GE_log("Changed active viewport to " + std::to_string(index));
    });
    //connect(viewport_docks[0], &Dock_Widget::window_closed, viewports[0], &Viewport::close);
    //connect(viewports[0], &Viewport::window_closed, viewport_closed);

    viewports[1] = new Viewport(1, context);
    viewport_docks[1] = new Dock_Widget;
    viewport_docks[1]->setWidget(viewports[1]);
    addDockWidget(Qt::TopDockWidgetArea, viewport_docks[1]);
    connect(viewports[1], &Viewport::made_active, [this](int32_t const index) {
        shared_state.active_editor = index;
        GE_log("Changed active viewport to " + std::to_string(index));
    });
    //connect(viewport_docks[1], &Dock_Widget::window_closed, viewports[1], &Viewport::close);
    //connect(viewports[1], &Viewport::window_closed, viewport_closed);

    outliner = new Outliner;
    outliner_dock = new Dock_Widget;
    outliner_dock->setObjectName("Outliner Dock");
    outliner_dock->setWidget(outliner);
    addDockWidget(Qt::BottomDockWidgetArea, outliner_dock);

    auto on_entity_selected = [this](Entity const entity, bool clear_previous_selection) {
        if (clear_previous_selection) {
            shared_state.selected_entities.clear();
        }

        shared_state.selected_entities.insert_unsorted(shared_state.selected_entities.cbegin(), entity);
        outliner->select_entities(shared_state.selected_entities);
    };

    auto on_entity_deselected = [this](Entity const entity) {
        auto iter = anton_stl::find(shared_state.selected_entities.begin(), shared_state.selected_entities.end(), entity);
        shared_state.selected_entities.erase(iter, iter + 1);
        outliner->deselect_entity(entity);
    };

    connect(outliner, &Outliner::entity_selected, on_entity_selected);
    connect(outliner, &Outliner::entity_deselected, on_entity_deselected);
    connect(viewports[0], &Viewport::entity_selected, on_entity_selected);
    connect(viewports[0], &Viewport::entity_deselected, on_entity_deselected);
    connect(viewports[1], &Viewport::entity_selected, on_entity_selected);
    connect(viewports[1], &Viewport::entity_deselected, on_entity_deselected);

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

    gizmo::terminate();
    delete context;
    delete surface;
    delete input_filter;
}

void Editor_Window::update() {
    outliner->update();

    auto const state = Input::get_key_state(Key::u);
    if (state.up_down_transitioned && !state.down) {
        if (!viewports[shared_state.active_editor]->is_cursor_locked()) {
            viewports[shared_state.active_editor]->lock_cursor_at(QCursor::pos().x(), QCursor::pos().y());
        } else {
            viewports[shared_state.active_editor]->unlock_cursor();
        }
    }

    auto viewport_camera_view = Editor::get_ecs().access<Viewport_Camera, Camera, Transform>();
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
    auto viewport_camera_view = Editor::get_ecs().access<Viewport_Camera, Camera, Transform>();
    for (Entity const entity: viewport_camera_view) {
        auto [viewport_camera, camera, transform] = viewport_camera_view.get<Viewport_Camera, Camera, Transform>(entity);
        auto viewport = viewports[viewport_camera.viewport_index];
        if (viewport) {
            Matrix4 const view_mat = get_camera_view_matrix(transform);
            Matrix4 const projection_mat = get_camera_projection_matrix(camera, viewport->width(), viewport->height());
            viewport->render(view_mat, projection_mat, transform, shared_state.selected_entities);
        }
    }
}
