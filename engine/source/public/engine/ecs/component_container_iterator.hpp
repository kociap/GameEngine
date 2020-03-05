#ifndef ENGINE_ECS_COMPONENT_CONTAINER_ITERATOR_HPP_INCLUDE
#define ENGINE_ECS_COMPONENT_CONTAINER_ITERATOR_HPP_INCLUDE

#include <core/types.hpp>
#include <core/stl/iterators.hpp>
#include <core/stl/type_traits.hpp>

namespace anton_engine {
    template <typename T>
    class Component_Container;

    // TODO: Visualiser
    template <typename Component>
    class Component_Container_Iterator {
    public:
        using value_type = Component;
        using reference = Component&;
        using pointer = Component*;
        using difference_type = isize;
        using iteragor_category = anton_stl::Contiguous_Iterator_Tag;

        Component_Container_Iterator() = delete;
        Component_Container_Iterator(Component_Container_Iterator const&) = default;
        Component_Container_Iterator(Component_Container_Iterator&&) noexcept = default;
        ~Component_Container_Iterator() = default;
        Component_Container_Iterator& operator=(Component_Container_Iterator const&) = default;
        Component_Container_Iterator& operator=(Component_Container_Iterator&&) noexcept = default;

        Component_Container_Iterator& operator++() {
            ++index;
            return *this;
        }

        [[nodiscard]] Component_Container_Iterator operator++(int) {
            Component_Container_Iterator copy(*this);
            ++index;
            return copy;
        }

        Component_Container_Iterator& operator--() {
            --index;
            return *this;
        }

        [[nodiscard]] Component_Container_Iterator operator--(int) {
            Component_Container_Iterator copy(*this);
            --index;
            return copy;
        }

        Component_Container_Iterator& operator+=(difference_type n) {
            index += n;
            return *this;
        }

        Component_Container_Iterator& operator-=(difference_type n) {
            index -= n;
            return *this;
        }

        [[nodiscard]] Component* operator->() {
            if constexpr (anton_stl::is_empty<Component>) {
                return data;
            } else {
                return data + index;
            }
        }

        [[nodiscard]] Component const* operator->() const {
            if constexpr (anton_stl::is_empty<Component>) {
                return data;
            } else {
                return data + index;
            }
        }

        [[nodiscard]] Component& operator*() {
            if constexpr (anton_stl::is_empty<Component>) {
                return *data;
            } else {
                return *(data + index);
            }
        }

        [[nodiscard]] Component const& operator*() const {
            if constexpr (anton_stl::is_empty<Component>) {
                return *data;
            } else {
                return *(data + index);
            }
        }

        [[nodiscard]] bool operator==(Component_Container_Iterator const& rhs) {
            return index == rhs.index;
        }

        [[nodiscard]] bool operator<(Component_Container_Iterator const& rhs) {
            return index < rhs.index;
        }

    private:
        Component* data;
        difference_type index;

        Component_Container_Iterator(Component* p, difference_type i): data(p), index(i) {}

        friend class Component_Container<Component>;
    };

    template <typename Component>
    [[nodiscard]] inline Component_Container_Iterator<Component> operator+(Component_Container_Iterator<Component> lhs,
                                                                           typename Component_Container_Iterator<Component>::difference_type n) {
        lhs += n;
        return lhs;
    }

    template <typename Component>
    [[nodiscard]] inline Component_Container_Iterator<Component> operator+(typename Component_Container_Iterator<Component>::difference_type n,
                                                                           Component_Container_Iterator<Component> rhs) {
        rhs += n;
        return rhs;
    }

    template <typename Component>
    [[nodiscard]] inline Component_Container_Iterator<Component> operator-(Component_Container_Iterator<Component> lhs,
                                                                           typename Component_Container_Iterator<Component>::difference_type n) {
        lhs -= n;
        return lhs;
    }

    template <typename Component>
    [[nodiscard]] inline bool operator!=(Component_Container_Iterator<Component> const& lhs, Component_Container_Iterator<Component> const& rhs) {
        return !(lhs == rhs);
    }

    template <typename Component>
    [[nodiscard]] inline bool operator>(Component_Container_Iterator<Component> const& lhs, Component_Container_Iterator<Component> const& rhs) {
        return rhs < lhs;
    }

    template <typename Component>
    [[nodiscard]] inline bool operator<=(Component_Container_Iterator<Component> const& lhs, Component_Container_Iterator<Component> const& rhs) {
        return !(lhs > rhs);
    }

    template <typename Component>
    [[nodiscard]] inline bool operator>=(Component_Container_Iterator<Component> const& lhs, Component_Container_Iterator<Component> const& rhs) {
        return !(lhs < rhs);
    }
} // namespace anton_engine

#endif // !ENGINE_ECS_COMPONENT_CONTAINER_ITERATOR_HPP_INCLUDE
