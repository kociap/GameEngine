#include <outliner_entity_list.hpp>

#include <anton_stl/type_traits.hpp>
#include <anton_stl/utility.hpp>

ANTON_DISABLE_WARNINGS()
#include <QColor>
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QStyleOption>
#include <QVBoxLayout>
ANTON_RESTORE_WARNINGS()

Outliner_Item::Outliner_Item(Entity e, anton_stl::String_View str, QWidget* parent): QWidget(parent), entity(e) {
    setAutoFillBackground(true);
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
    QPalette selected_palette;
    selected_palette.setColor(QPalette::Window, QColor(252, 175, 32));
    setPalette(selected_palette);
    // setStyleSheet("background-color: #FCAF20;");
    _selected = true;
}

void Outliner_Item::deselect() {
    QPalette selected_palette;
    selected_palette.setColor(QPalette::Window, QColor(25, 25, 25));
    setPalette(selected_palette);
    _selected = false;
}

bool Outliner_Item::is_selected() const {
    return _selected;
}

void Outliner_Item::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        if (!is_selected()) {
            Q_EMIT selected(entity, true);
        }
    }
}

void Outliner_Item::paintEvent(QPaintEvent*) {
    QStyleOption option;
    option.init(this);
    QPainter painter(this);
    QBrush brush(QColor{252, 175, 32});
    painter.setBrush(brush);
    style()->drawPrimitive(QStyle::PE_Widget, &option, &painter, this);
}
