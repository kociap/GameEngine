#include <outliner_item.hpp>

#include <anton_stl/type_traits.hpp>
#include <anton_stl/utility.hpp>
#include <editor_events.hpp>

ANTON_DISABLE_WARNINGS();
#include <QColor>
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QResizeEvent>
#include <QStyleOption>
#include <QVBoxLayout>
ANTON_RESTORE_WARNINGS();

namespace anton_engine {
    Outliner_Item::Outliner_Item(Entity e, anton_stl::String_View str, QWidget* parent): QWidget(parent), entity(e) {
        setAutoFillBackground(true);
        setMinimumHeight(32);
        label = new QLabel(QString::fromUtf8(str.data()), this);
        label->setIndent(15);
        label->setMargin(0);
        label->show();
    }

    Outliner_Item::Outliner_Item(Outliner_Item&& other) noexcept: label(other.label), entity(other.entity) {
        other.label = nullptr;
        other.entity = null_entity;

        setAutoFillBackground(true);
        label->setParent(this);

        if (QWidget* parent_widget = other.parentWidget()) {
            setParent(parent_widget);
            other.setParent(nullptr);
        }

        if (other.is_selected()) {
            select();
        }

        show();
    }

    Outliner_Item& Outliner_Item::operator=(Outliner_Item&& other) noexcept {
        using anton_stl::swap;
        swap(label, other.label);
        label->setParent(this);
        other.label->setParent(&other);
        swap(entity, other.entity);

        if (QWidget* parent_widget = other.parentWidget()) {
            setParent(parent_widget);
            other.setParent(nullptr);
        }

        if (other.is_selected()) {
            select();
        } else {
            deselect();
        }

        return *this;
    }

    Outliner_Item::~Outliner_Item() {}

    Entity Outliner_Item::get_associated_entity() const {
        return entity;
    }

    void Outliner_Item::set_name(anton_stl::String_View name) {
        label->setText(QString::fromUtf8(name.data()));
    }

    void Outliner_Item::select() {
        QPalette selected_palette;
        auto role = backgroundRole();
        selected_palette.setColor(role, QColor(252, 175, 32));
        setPalette(selected_palette);
        // setStyleSheet("background-color: #FCAF20;");
        _selected = true;
    }

    void Outliner_Item::deselect() {
        QPalette selected_palette;
        auto role = backgroundRole();
        selected_palette.setColor(role, QColor(25, 25, 25));
        setPalette(selected_palette);
        _selected = false;
    }

    bool Outliner_Item::is_selected() const {
        return _selected;
    }

    void Outliner_Item::mouseReleaseEvent(QMouseEvent* event) {
        if (event->button() == Qt::LeftButton) {
            if (!is_selected()) {
                editor_events::entity_selected(entity, true);
            }
        }
    }

    void Outliner_Item::resizeEvent(QResizeEvent*) {
        label->setGeometry(0, 0, width(), height());
    }

    void Outliner_Item::paintEvent(QPaintEvent*) {
        QStyleOption option;
        option.init(this);
        QPainter painter(this);
        QBrush brush(QColor{252, 175, 32});
        painter.setBrush(brush);
        style()->drawPrimitive(QStyle::PE_Widget, &option, &painter, this);
    }
} // namespace anton_engine
