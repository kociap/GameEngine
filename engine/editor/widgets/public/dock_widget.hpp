#ifndef EDITOR_MISC_DOCK_WIDGET_HPP_INCLUDE
#define EDITOR_MISC_DOCK_WIDGET_HPP_INCLUDE

#include <diagnostic_macros.hpp>
ANTON_DISABLE_WARNINGS()
#include <QDockWidget>
ANTON_RESTORE_WARNINGS()

class QCloseEvent;

namespace anton_engine {
    class Dock_Widget: public QDockWidget {
        Q_OBJECT

    public:
        using QDockWidget::QDockWidget;

    protected:
        void closeEvent(QCloseEvent*) override;

    Q_SIGNALS:
        void window_closed();
    };
} // namespace anton_engine

#endif // !EDITOR_MISC_DOCK_WIDGET_HPP_INCLUDE
