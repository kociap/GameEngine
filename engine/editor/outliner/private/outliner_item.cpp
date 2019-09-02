#include <anton_stl/type_traits.hpp>
#include <anton_stl/utility.hpp>
#include <outliner_item.hpp>

ANTON_DISABLE_WARNINGS()
#include <QLabel>
#include <QPainter>
#include <QStyleOption>
#include <QVBoxLayout>
ANTON_RESTORE_WARNINGS()

Outliner_Item::Outliner_Item(Entity e, anton_stl::String_View str, QWidget* parent): QWidget(parent), entity(e) {
    layout = new QVBoxLayout(this);
    layout->setContentsMargins(10, 5, 10, 5);
    layout->setSpacing(0);
    label = new QLabel(QString::fromUtf8(str.data()));
    label->setIndent(15);
    label->setMargin(0);
    layout->addWidget(label);
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

void Outliner_Item::select() {
    setStyleSheet("background-color: #FCAF20;");
}

void Outliner_Item::deselect() {
    setStyleSheet("background-color: #FFFFFF;");
}

void Outliner_Item::paintEvent(QPaintEvent*) {
    QStyleOption option;
    option.init(this);
    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &option, &painter, this);
}
