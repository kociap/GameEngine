#ifndef ENGINE_ECS_COMPONENT_ACCESS_HPP_INCLUDE
#define ENGINE_ECS_COMPONENT_ACCESS_HPP_INCLUDE

#include "ecs/component_container.hpp"

#include <algorithm>
#include <tuple>
#include <type_traits>

template <typename... Components>
class Component_Access {
    static_assert(sizeof...(Components) > 1);

    friend class ECS;

    template <typename T>
    using container_type = Component_Container<T>;

    Component_Access(container_type<Components>*... c): containers(c...) {}

public:
    using size_type = Component_Container_Base::size_type;

    class iterator {
        friend class Component_Access;

        using underlying_iterator_t = typename Component_Container_Base::iterator;

        iterator(std::tuple<container_type<Components>*...> c, underlying_iterator_t b, underlying_iterator_t e): containers(c), begin(b), end(e) {}

    public:
        using value_type = typename underlying_iterator_t::value_type;
        using reference = typename underlying_iterator_t::reference;
        using pointer = typename underlying_iterator_t::pointer;
        using difference_type = typename underlying_iterator_t::difference_type;
        using iterator_category = std::forward_iterator_tag;

        iterator& operator++() {
            return (++begin != end && !has_all_components(*begin)) ? ++(*this) : *this;
        }

        // clang-format off
        pointer operator->() { return begin.operator->(); }
        reference operator*() { return *begin; }
        friend bool operator==(iterator const& a, iterator const& b) { return a.begin == b.begin; }
        friend bool operator!=(iterator const& a, iterator const& b) { return a.begin != b.begin; }
        // clang-format on

    private:
        bool has_all_components(Entity entity) {
            return (... && std::get<container_type<Components>*>(containers)->has(entity));
        }

    private:
        std::tuple<container_type<Components>*...> containers;
        underlying_iterator_t begin;
        underlying_iterator_t end;
    };

public:
    [[nodiscard]] size_type size() const {
        return std::min({std::get<container_type<Components>*>(containers)->size()...});
    }

    [[nodiscard]] iterator begin() {
        auto c = find_smallest_container();
        return iterator(containers, c->begin(), c->end());
    }

    [[nodiscard]] iterator end() {
        auto c = find_smallest_container();
        return iterator(containers, c->end(), c->end());
    }

    template <typename Callable>
    void each(Callable&& callable) {
        static_assert(std::is_invocable_v<Callable, Entity, Components&...> || std::is_invocable_v<Callable, Components&...>);
        auto smallest_container = find_smallest_container();
        for (Entity const entity: *smallest_container) {
            if (has_all_components(entity)) {
                if constexpr (std::is_invocable_v<Callable, Components&...>) {
                    callable(get<Components>(entity)...);
                } else {
                    callable(entity, get<Components>(entity)...);
                }
            }
        }
    }

    template <typename... T>
    [[nodiscard]] decltype(auto) get(Entity const entity) {
        if constexpr (sizeof...(T) == 1) {
            return (..., std::get<container_type<T>*>(containers)->get(entity));
        } else {
            return std::tuple<T&...>(get<T>(entity)...);
        }
    }

private:
    bool has_all_components(Entity entity) {
        return (... && std::get<container_type<Components>*>(containers)->has(entity));
    }

    // TODO add const support
    Component_Container_Base* find_smallest_container() {
        // clang-format off
        return std::min({static_cast<Component_Container_Base*>(std::get<container_type<Components>*>(containers))...}, [](auto* a, auto* b) { 
            return a->size() < b->size(); 
        });
        // clang-format on
    }

private:
    std::tuple<container_type<Components>*...> containers;
};

// Specialization of Component_Access for single component type.
// Avoids unnecessary checks, which results in performance boost
template <typename Component>
class Component_Access<Component> {
    friend class ECS;

    template <typename T>
    using container_type = Component_Container<T>;

    Component_Access(container_type<Component>* c): container(c) {}

public:
    using size_type = Component_Container_Base::size_type;
    using iterator = Component_Container_Base::iterator;

    [[nodiscard]] size_type size() const {
        return container->size();
    }

    [[nodiscard]] iterator begin() {
        return container->Component_Container_Base::begin();
    }

    [[nodiscard]] iterator end() {
        return container->Component_Container_Base::end();
    }

    template <typename Callable>
    void each(Callable&& callable) {
        static_assert(std::is_invocable_v<Callable, Entity, Component&> || std::is_invocable_v<Callable, Component&>);
        for (Entity entity: *static_cast<Component_Container_Base*>(container)) {
            if constexpr (std::is_invocable_v<Callable, Component&>) {
                callable(get<Component>(entity));
            } else {
                callable(entity, get<Component>(entity));
            }
        }
    }

    [[nodiscard]] Component& get(Entity const entity) {
        return container->get(entity);
    }

private:
    container_type<Component>* container;
};

#endif // !ENGINE_ECS_COMPONENT_ACCESS_HPP_INCLUDE
