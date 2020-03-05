#ifndef EDITOR_OUTLINER_OUTLINER_ITEM_HPP_INCLUDE
#define EDITOR_OUTLINER_OUTLINER_ITEM_HPP_INCLUDE

#include <core/stl/string_view.hpp>
#include <core/stl/vector.hpp>
#include <engine/ecs/entity.hpp>

#include <core/diagnostic_macros.hpp>
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
        void set_name(anton_stl::String_View);

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
