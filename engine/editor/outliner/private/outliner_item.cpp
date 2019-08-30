#include <anton_stl/type_traits.hpp>
#include <anton_stl/utility.hpp>
#include <outliner_item.hpp>

ANTON_DISABLE_WARNINGS()
#include <QLabel>
ANTON_RESTORE_WARNINGS()

Outliner_Item::Outliner_Item(Entity e, anton_stl::String_View str, QWidget* parent): QWidget(parent), entity(e) {
    setMinimumHeight(32);
    label = new QLabel(str.data(), this);
    label->show();
}

Outliner_Item::Outliner_Item(Outliner_Item&& other) noexcept: label(other.label), entity(other.entity) {
    other.label = nullptr;
    other.entity = null_entity;
}

Outliner_Item& Outliner_Item::operator=(Outliner_Item&& other) noexcept {
    using anton_stl::swap;
    swap(label, other.label);
    swap(entity, other.entity);
    return *this;
}

Outliner_Item::~Outliner_Item() {
    delete label;
}

Entity Outliner_Item::get_associated_entity() const {
    return entity;
}
