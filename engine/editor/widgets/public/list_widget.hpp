#ifndef EDITOR_WIDGETS_BOX_LAYOUT_HPP_INCLUDE
#define EDITOR_WIDGETS_BOX_LAYOUT_HPP_INCLUDE

#include <anton_stl/vector.hpp>
#include <math/math.hpp>

#include <diagnostic_macros.hpp>
ANTON_DISABLE_WARNINGS();
#include <QEvent>
#include <QResizeEvent>
#include <QSpacerItem>
#include <QWidget>
ANTON_RESTORE_WARNINGS();

namespace anton_engine {
    template <typename T>
    class List_Widget: public QWidget {
    public:
        using iterator = typename anton_stl::Vector<T>::iterator;
        using const_iterator = typename anton_stl::Vector<T>::const_iterator;

    public:
        explicit List_Widget(QWidget* parent = nullptr): QWidget(parent), stretch_items(false) {}
        explicit List_Widget(bool stretch_items, QWidget* parent = nullptr): QWidget(parent), stretch_items(stretch_items) {}
        // List_Widget(List_Widget&&) noexcept;
        ~List_Widget() override {}

        iterator begin() {
            return items.begin();
        }

        const_iterator begin() const {
            return items.begin();
        }

        iterator end() {
            return items.end();
        }

        const_iterator end() const {
            return items.end();
        }

        template <typename T>
        T& push_back(T&& item) {
            T& item = items.push_back(anton_stl::forward<T>(item));
            item.show();
            recalculate_layout();
            return item;
        }

        template <typename... Args>
        T& emplace_back(Args&&... args) {
            T& item = items.emplace_back(anton_stl::forward<Args>(args)..., this);
            item.show();
            recalculate_layout();
            return item;
        }

        iterator erase(const_iterator first, const_iterator last) {
            auto iter = items.erase(first, last);
            recalculate_layout();
            return iter;
        }

        void set_spacing(uint32_t s) {
            spacing = s;
            recalculate_layout();
        }

    protected:
        bool event(QEvent* e) override {
            switch (e->type()) {
                case QEvent::Resize:
                case QEvent::LayoutRequest:
                    recalculate_layout();
                    return true;

                default:
                    return false;
            }
        }

    private:
        void recalculate_layout() {
            if (items.size() == 0) {
                return;
            }

            uint32_t current_height = 0;
            uint32_t effective_height = (height() - spacing * (items.size() - 1));
            uint32_t remainder = effective_height % items.size();
            uint32_t minimum_height = 0;
            for (auto& item: items) {
                uint32_t item_height = math::max((int64_t)item.minimumHeight(), effective_height / items.size());
                if (remainder > 0) {
                    item_height += 1;
                    remainder -= 1;
                }
                item.setGeometry(0, current_height, width(), item_height);
                current_height += item_height + spacing;
                minimum_height += item.minimumHeight() + spacing;
            }

            setMinimumHeight(minimum_height - spacing);
        }

    private:
        anton_stl::Vector<T> items;
        uint32_t spacing = 0;
        bool stretch_items;
    };
} // namespace anton_engine

#endif // !EDITOR_WIDGETS_BOX_LAYOUT_HPP_INCLUDE
