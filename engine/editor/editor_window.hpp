#ifndef EDITOR_EDITOR_WINDOW_HPP_INCLUDE
#define EDITOR_EDITOR_WINDOW_HPP_INCLUDE

#include <containers/vector.hpp>
#include <ecs/entity.hpp>

#include <QMainWindow>
#include <memory>

namespace Ui {
    class Editor_Window;
}

class User_Input_Filter;
class Viewport;
class QOpenGLContext;
class QOffscreenSurface;
class QMouseEvent;
class Dock_Widget;

class Editor_Shared_State {
public:
    containers::Vector<Entity> selected_entities;
    int32_t active_editor = 0;
};

class Editor_Window: public QMainWindow {
    Q_OBJECT

public:
    explicit Editor_Window(QWidget* parent = nullptr);
    ~Editor_Window();

    void update();
    void render();

private:
    containers::Vector<Viewport*> viewports;
    containers::Vector<Dock_Widget*> viewport_docks;
    Ui::Editor_Window* ui = nullptr;
    QOpenGLContext* context;
    QOffscreenSurface* surface;
    User_Input_Filter* input_filter;
    Editor_Shared_State shared_state;
};

#endif // !EDITOR_EDITOR_WINDOW_HPP_INCLUDE
