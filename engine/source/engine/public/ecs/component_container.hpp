#ifndef ENGINE_ECS_COMPONENT_CONTAINER_HPP_INCLUDE
#define ENGINE_ECS_COMPONENT_CONTAINER_HPP_INCLUDE

#include <containers/vector.hpp>
#include <debug_macros.hpp>
#include <ecs/entity.hpp>
#include <iterators.hpp>
#include <serialization/archives/binary.hpp>

#include <type_traits>

class Component_Container_Base {
public:
    using size_type = uint64_t;
    using pointer = containers::Vector<Entity>::pointer;
    using const_pointer = containers::Vector<Entity>::const_pointer;
    using iterator = containers::Vector<Entity>::iterator;
    using const_iterator = containers::Vector<Entity>::const_iterator;

protected:
    constexpr static size_type npos = static_cast<size_type>(-1);

    static void serialize(serialization::Binary_Output_Archive&, Component_Container_Base const*);
    static void deserialize(serialization::Binary_Input_Archive&, Component_Container_Base*&);

public:
    virtual ~Component_Container_Base();

    [[nodiscard]] pointer data();
    [[nodiscard]] const_pointer data() const;

    [[nodiscard]] iterator begin();
    [[nodiscard]] iterator end();

    [[nodiscard]] bool has(Entity entity);
    [[nodiscard]] size_type size() const;

protected:
    void add_entity(Entity entity);
    size_type get_index(Entity entity);
    void remove_entity(Entity entity);

private:
    size_type indirect_index(Entity entity);
    void ensure(size_type index);

private:
    containers::Vector<size_type> indirect;
    containers::Vector<Entity> entities;
};

template <typename Component>
class Component_Container: public Component_Container_Base {
private:
    using base_t = Component_Container_Base;

    template <bool = std::is_empty_v<Component>>
    class Iterator: public iterators::iterator<Component_Container<Component>> {
        friend class Component_Container<Component>;

        using base_type = iterators::iterator<Component_Container<Component>>;
        Iterator(Component* p, size_type i): base_type(p + i) {}
    };

    template <>
    class Iterator<true> {
        friend class Component_Container<Component>;

        Iterator(Component* p, size_type i): ptr(p), index(i) {}

    public:
        using value_type = Component;
        using reference = Component&;
        using pointer = Component*;

        Iterator& operator++() {
            ++index;
            return *this;
        }

        Iterator& operator--() {
            --index;
            return *this;
        }

        // clang-format off
        reference operator*() { return *ptr; }
        pointer operator->() { return ptr; }
        friend bool operator==(Iterator const& a, Iterator const& b) { return a.index == b.index; }
        friend bool operator!=(Iterator const& a, Iterator const& b) { return a.index != b.index; }
        friend bool operator<(Iterator const& a, Iterator const& b) { return a.index < b.index; }
        friend bool operator>(Iterator const& a, Iterator const& b) { return a.index > b.index; }
        friend bool operator<=(Iterator const& a, Iterator const& b) { return a.index <= b.index; }
        friend bool operator>=(Iterator const& a, Iterator const& b) { return a.index >= b.index; }
        // clang-format on

    private:
        Component* ptr;
        size_type index;
    };

public:
    using iterator = Iterator<>;

    static void serialize(serialization::Binary_Output_Archive& archive, Component_Container_Base const*);
    static void deserialize(serialization::Binary_Input_Archive& archive, Component_Container_Base*&);

    virtual ~Component_Container() {}

    [[nodiscard]] Component const* raw() const {
        if constexpr (std::is_empty_v<Component>) {
            return &components;
        } else {
            return components.data();
        }
    }

    [[nodiscard]] Component* raw() {
        return const_cast<Component*>(const_cast<Component_Container const&>(*this).raw());
    }

    [[nodiscard]] iterator begin() {
        return {raw, 0};
    }

    [[nodiscard]] iterator end() {
        return {raw, size()};
    }

    template <typename... Args>
    Component& add(Entity const entity, Args&&... args) {
        GE_assert(!has(entity), "Attempting to add duplicate entity");
        if constexpr (std::is_empty_v<Component>) {
            add_entity(entity);
            return components;
        } else {
            components.emplace_back(std::forward<Args>(args)...);
            add_entity(entity);
            return components.back();
        }
    }

    void remove(Entity const entity) {
        if constexpr (!std::is_empty_v<Component>) {
            components.erase_unsorted(get_index(entity));
        }

        remove_entity(entity);
    }

    [[nodiscard]] Component& get(Entity const entity) {
        GE_assert(has(entity), "Attempting to get component of an entity that has not been registered");
        if constexpr (std::is_empty_v<Component>) {
            return components;
        } else {
            return components[get_index(entity)];
        }
    }

    [[nodiscard]] Component* try_get(Entity const entity) {
        if constexpr (std::is_empty_v<Component>) {
            return has(entity) ? &components : nullptr;
        } else {
            return has(entity) ? components.data() + get_index(entity) : nullptr;
        }
    }

private:
    std::conditional_t<std::is_empty_v<Component>, Component, containers::Vector<Component>> components;
};

#include "component_container.tpp"

#endif // !ENGINE_ECS_COMPONENT_CONTAINER_HPP_INCLUDE
