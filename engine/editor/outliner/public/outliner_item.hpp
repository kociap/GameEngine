#ifndef EDITOR_OUTLINER_OUTLINER_ITEM_HPP_INCLUDE
#define EDITOR_OUTLINER_OUTLINER_ITEM_HPP_INCLUDE

#include <anton_stl/string_view.hpp>
#include <anton_stl/vector.hpp>
#include <ecs/entity.hpp>

#include <diagnostic_macros.hpp>
ANTON_DISABLE_WARNINGS();
#include <QWidget>
ANTON_RESTORE_WARNINGS();

class QLabel;
class QPaintEvent;
class QMouseEvent;
class QResizeEvent;

namespace anton_engine {
    class Outliner_Item: public QWidget {
        Q_OBJECT

    public:
        explicit Outliner_Item(Entity, anton_stl::String_View, QWidget* parent = nullptr);
        Outliner_Item(Outliner_Item&&) noexcept;
        Outliner_Item& operator=(Outliner_Item&&) noexcept;
        ~Outliner_Item() override;

        Entity get_associated_entity() const;

        void select();
        void deselect();
        bool is_selected() const;

    Q_SIGNALS:
        void selected(Entity associated_entity, bool clear_previous_selection);
        void deselected(Entity associated_entity);

    protected:
        void mouseReleaseEvent(QMouseEvent*) override;
        void resizeEvent(QResizeEvent*) override;
        void paintEvent(QPaintEvent*) override;

    private:
        QLabel* label;
        Entity entity;
        bool _selected = false;
    };
} // namespace anton_engine

#endif // !EDITOR_OUTLINER_OUTLINER_ITEM_HPP_INCLUDE
