#ifndef EDITOR_MISC_DOCK_WIDGET_HPP_INCLUDE
#define EDITOR_MISC_DOCK_WIDGET_HPP_INCLUDE

#include <QDockWidget>

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
