#ifndef EDITOR_EDITOR_WINDOW_HPP_INCLUDE
#define EDITOR_EDITOR_WINDOW_HPP_INCLUDE

#include <containers/vector.hpp>
#include <ecs/entity.hpp>

#include <QMainWindow>
#include <memory>

namespace Ui {
    class Editor_Window;
}

class Viewport;
class Level_Editor_Shared_State;
class QOpenGLContext;
class QOffscreenSurface;

class Editor_Shared_State {
public:
    containers::Vector<Entity> selected_entities;
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
    containers::Vector<QDockWidget*> viewport_docks;
    Ui::Editor_Window* ui = nullptr;
    Level_Editor_Shared_State* level_editor_state = nullptr;
    QOpenGLContext* context;
	QOffscreenSurface* surface;
    Editor_Shared_State shared_state;
};

#endif // !EDITOR_EDITOR_WINDOW_HPP_INCLUDE
