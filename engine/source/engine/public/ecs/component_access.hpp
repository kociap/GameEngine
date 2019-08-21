#ifndef ENGINE_ECS_COMPONENT_ACCESS_HPP_INCLUDE
#define ENGINE_ECS_COMPONENT_ACCESS_HPP_INCLUDE

#include <ecs/component_container.hpp>

#include <algorithm>
#include <tuple>
#include <type_traits>

template <typename... Components>
class Component_Access {
    static_assert(sizeof...(Components) > 0, "Why would you do this?");

    friend class ECS;

    Component_Access(Component_Container<Components>*... c): containers(c...) {}

public:
    using size_type = Component_Container_Base::size_type;

    class iterator {
        friend class Component_Access;

        using underlying_iterator_t = typename Component_Container_Base::iterator;

        // begin and end are iterators into the smallest container
        iterator(std::tuple<Component_Container<Components>*...> c, underlying_iterator_t b, underlying_iterator_t e): containers(c), begin(b), end(e) {}

    public:
        using value_type = typename anton_stl::iterator_traits<underlying_iterator_t>::value_type;
        using reference = typename anton_stl::iterator_traits<underlying_iterator_t>::reference;
        using pointer = typename anton_stl::iterator_traits<underlying_iterator_t>::pointer;
        using difference_type = typename anton_stl::iterator_traits<underlying_iterator_t>::difference_type;
        using iterator_category = std::forward_iterator_tag;

        iterator& operator++() {
            return (++begin != end && !has_all_components(*begin)) ? ++(*this) : *this;
        }

        // clang-format off
        // Return underlying_iterator_t to call operator-> recursively
        underlying_iterator_t operator->() { return begin; }
        reference operator*() { return *begin; }
        friend bool operator==(iterator const& a, iterator const& b) { return a.begin == b.begin; }
        friend bool operator!=(iterator const& a, iterator const& b) { return a.begin != b.begin; }
        // clang-format on

    private:
        bool has_all_components(Entity entity) {
            return (... && std::get<Component_Container<Components>*>(containers)->has(entity));
        }

    private:
        std::tuple<Component_Container<Components>*...> containers;
        underlying_iterator_t begin;
        underlying_iterator_t end;
    };

public:
    [[nodiscard]] size_type size() const {
        return std::min({std::get<Component_Container<Components>*>(containers)->size()...});
    }

    [[nodiscard]] iterator begin() {
        auto c = find_smallest_container();
        return iterator(containers, c->begin(), c->end());
    }

    [[nodiscard]] iterator end() {
        auto c = find_smallest_container();
        return iterator(containers, c->end(), c->end());
    }

    template <typename... T>
    [[nodiscard]] decltype(auto) get(Entity const entity) {
        if constexpr (sizeof...(T) == 1) {
            return (..., std::get<Component_Container<T>*>(containers)->get(entity));
        } else {
            return std::tuple<T&...>(get<T>(entity)...);
        }
    }

    // Provides a convenient way to iterate over all entities and their components.
    // Requires a callable of form void(Components&...) or void(Entity, Components&...)
    // May be less efficient than range-based for loop or other alternatives because it prefetches all components
    //
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

private:
    bool has_all_components(Entity entity) {
        return (... && std::get<Component_Container<Components>*>(containers)->has(entity));
    }

    // TODO add const support
    Component_Container_Base* find_smallest_container() {
        // clang-format off
        return std::min({static_cast<Component_Container_Base*>(std::get<Component_Container<Components>*>(containers))...}, [](auto* a, auto* b) { 
            return a->size() < b->size(); 
        });
        // clang-format on
    }

private:
    std::tuple<Component_Container<Components>*...> containers;
};

// Specialization of Component_Access for single component type.
// Avoids unnecessary checks, which results in performance boost.
template <typename Component>
class Component_Access<Component> {
    friend class ECS;

    Component_Access(Component_Container<Component>* c): container(c) {}

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

    [[nodiscard]] Component& get(Entity const entity) {
        return container->get(entity);
    }

    // Provides a convenient way to iterate over all entities and their components.
    // Requires a callable of form void(Component&) or void(Entity, Component&)
    //
    template <typename Callable>
    void each(Callable&& callable) {
        static_assert(std::is_invocable_v<Callable, Entity, Component&> || std::is_invocable_v<Callable, Component&>);
        for (Entity entity: *static_cast<Component_Container_Base*>(container)) {
            if constexpr (std::is_invocable_v<Callable, Component&>) {
                callable(get(entity));
            } else {
                callable(entity, get(entity));
            }
        }
    }

private:
    Component_Container<Component>* container;
};

#endif // !ENGINE_ECS_COMPONENT_ACCESS_HPP_INCLUDE
