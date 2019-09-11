#ifndef EDITOR_EDITOR_WINDOW_HPP_INCLUDE
#define EDITOR_EDITOR_WINDOW_HPP_INCLUDE

#include <anton_stl/vector.hpp>
#include <diagnostic_macros.hpp>
#include <ecs/entity.hpp>

ANTON_DISABLE_WARNINGS()
#include <QMainWindow>
ANTON_RESTORE_WARNINGS()

class QOpenGLContext;
class QOffscreenSurface;
class QMouseEvent;

namespace anton_engine {
    class User_Input_Filter;
    class Viewport;
    class Dock_Widget;
    class Outliner;
    class Log_Viewer;

    class Editor_Shared_State {
    public:
        anton_stl::Vector<Entity> selected_entities;
        int32_t active_editor = 0;
    };

    class Editor_Window: public QMainWindow {
        Q_OBJECT

    public:
        explicit Editor_Window(QWidget* parent = nullptr);
        ~Editor_Window();

        void setup_interface();
        void update();
        void render();

        Outliner* outliner = nullptr;
        Log_Viewer* log_viewer = nullptr;
        Editor_Shared_State shared_state;

    private:
        anton_stl::Vector<Viewport*> viewports;
        anton_stl::Vector<Dock_Widget*> viewport_docks;
        Dock_Widget* outliner_dock = nullptr;
        Dock_Widget* log_viewer_dock = nullptr;
        QOpenGLContext* context;
        QOffscreenSurface* surface;
        User_Input_Filter* input_filter;
    };
} // namespace anton_engine

#endif // !EDITOR_EDITOR_WINDOW_HPP_INCLUDE
