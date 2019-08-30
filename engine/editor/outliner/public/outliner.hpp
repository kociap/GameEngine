#ifndef EDITOR_OUTLINER_OUTLINER_HPP_INCLUDE
#define EDITOR_OUTLINER_OUTLINER_HPP_INCLUDE

#include <anton_stl/vector.hpp>
#include <cstdint>
#include <diagnostic_macros.hpp>
#include <ecs/entity.hpp>
#include <outliner_item.hpp>

ANTON_DISABLE_WARNINGS();
#include <QWidget>
ANTON_RESTORE_WARNINGS();

class QVBoxLayout;

class Outliner: public QWidget {
    Q_OBJECT

public:
    Outliner();
    ~Outliner() override;

    void update();
    void remove_entities(anton_stl::Vector<Entity> const& entities_to_remove);

    int32_t indentation() const;
    void set_indentation(int32_t);

    QVBoxLayout* layout = nullptr;

private:
    anton_stl::Vector<Outliner_Item> items;
};

#endif // !EDITOR_OUTLINER_OUTLINER_HPP_INCLUDE
