#ifndef EDITOR_OUTLINER_OUTLINER_HPP_INCLUDE
#define EDITOR_OUTLINER_OUTLINER_HPP_INCLUDE

#include <anton_stl/vector.hpp>
#include <cstdint>
#include <diagnostic_macros.hpp>
#include <ecs/entity.hpp>
#include <outliner_item.hpp>

#include <list_widget.hpp>

ANTON_DISABLE_WARNINGS();
#include <QWidget>
ANTON_RESTORE_WARNINGS();

class QVBoxLayout;
class QScrollArea;

namespace anton_engine {
    class Outliner: public QWidget {
    public:
        Outliner();
        ~Outliner() override;

        void update();
        void add_entity(Entity);
        void remove_entities(anton_stl::Vector<Entity> const& entities_to_remove);
        void select_entities(anton_stl::Vector<Entity> const&);
        void select_entity(Entity);
        void deselect_entity(Entity);

        int32_t indentation() const;
        void set_indentation(int32_t);

        void sort_entities_by_name_ascending();
        void sort_entities_by_name_descending();

    private:
        Entity last_selected = null_entity;

        List_Widget<Outliner_Item>* list_widget = nullptr;
        QVBoxLayout* layout = nullptr;
        QScrollArea* scroll_area;
    };
} // namespace anton_engine

#endif // !EDITOR_OUTLINER_OUTLINER_HPP_INCLUDE
