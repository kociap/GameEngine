#include <dock_widget.hpp>

#include <QCloseEvent>

void Dock_Widget::closeEvent(QCloseEvent* e) {
    QDockWidget::closeEvent(e);
    Q_EMIT window_closed();
}