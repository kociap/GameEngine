#ifndef EDITOR_OUTLINER_OUTLINER_ENTITY_LIST_HPP_INCLUDE
#define EDITOR_OUTLINER_OUTLINER_ENTITY_LIST_HPP_INCLUDE

#include <anton_stl/string_view.hpp>
#include <anton_stl/vector.hpp>
#include <ecs/entity.hpp>

#include <diagnostic_macros.hpp>
ANTON_DISABLE_WARNINGS();
#include <QWidget>
ANTON_RESTORE_WARNINGS();

class QLabel;
class QVBoxLayout;
class QPaintEvent;
class QMouseEvent;

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
    void paintEvent(QPaintEvent*) override;

private:
    QLabel* label;
    QVBoxLayout* layout;
    Entity entity;
    bool _selected = false;
};

class Outliner_Entity_List: public QWidget {
    Q_OBJECT

public:
    Outliner_Entity_List();

    void add_entity(Entity);
    void add_entities(anton_stl::Vector<Entity> const&);

    void sort_entities_by_name_ascending();
    void sort_entities_by_name_descending();

private:
    anton_stl::Vector<Outliner_Item> items;
    QVBoxLayout* layout = nullptr;
};

#endif // !EDITOR_OUTLINER_OUTLINER_ENTITY_LIST_HPP_INCLUDE
