#ifndef EDITOR_MISC_DOCK_WIDGET_HPP_INCLUDE
#define EDITOR_MISC_DOCK_WIDGET_HPP_INCLUDE

#include <diagnostic_macros.hpp>
ANTON_DISABLE_WARNINGS()
#include <QDockWidget>
ANTON_RESTORE_WARNINGS()

class QCloseEvent;

class Dock_Widget: public QDockWidget {
    Q_OBJECT

public:
    using QDockWidget::QDockWidget;

protected:
    void closeEvent(QCloseEvent*) override;

Q_SIGNALS:
    void window_closed();
};

#endif // !EDITOR_MISC_DOCK_WIDGET_HPP_INCLUDE
