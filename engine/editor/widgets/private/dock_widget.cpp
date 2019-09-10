#include <dock_widget.hpp>

ANTON_DISABLE_WARNINGS();
#include <QCloseEvent>
ANTON_RESTORE_WARNINGS();

namespace anton_engine {
    void Dock_Widget::closeEvent(QCloseEvent* e) {
        QDockWidget::closeEvent(e);
        Q_EMIT window_closed();
    }
} // namespace anton_engine
