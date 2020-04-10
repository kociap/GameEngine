#ifndef ENGINE_ECS_COMPONENT_VIEW_HPP_INCLUDE
#define ENGINE_ECS_COMPONENT_VIEW_HPP_INCLUDE

#include <core/math/math.hpp>
#include <core/atl/utility.hpp>
#include <engine/ecs/component_container.hpp>
#include <core/atl/tuple.hpp>

namespace anton_engine {
    template <typename... Components>
    class Component_View {
        static_assert(sizeof...(Components) > 0, "Why would you do this?");

        friend class ECS;

        Component_View(Component_Container<Components>*... c): _containers(c...) {}

    public:
        using size_type = Component_Container_Base::size_type;

        class iterator {
            friend class Component_View;

            using underlying_iterator_t = typename Component_Container_Base::iterator;

            // begin and end are iterators into the smallest container
            iterator(atl::Tuple<Component_Container<Components>*...> c, underlying_iterator_t b, underlying_iterator_t e): _containers(c), _begin(b), _end(e) {}

        public:
            using value_type = typename atl::Iterator_Traits<underlying_iterator_t>::value_type;
            using reference = typename atl::Iterator_Traits<underlying_iterator_t>::reference;
            using pointer = typename atl::Iterator_Traits<underlying_iterator_t>::pointer;
            using difference_type = typename atl::Iterator_Traits<underlying_iterator_t>::difference_type;
            using iterator_category = atl::Forward_Iterator_Tag;

            iterator& operator++() {
                ANTON_ASSERT(_begin != _end, "Cannot advance end iterator");
                do {
                    ++_begin;
                } while(_begin != _end && !has_all_components(*_begin));
                return *this;
            }

            iterator& operator+=(i64 rhs) {
                ANTON_VERIFY(rhs >= 0, "Forward iterator may not be moved backwards.");
                for (; rhs > 0; --rhs) {
                    ++(*this);
                }
            }

            underlying_iterator_t operator->() {
                return _begin;
            }

            reference operator*() {
                return *_begin;
            }

            [[nodiscard]] friend iterator operator+(iterator lhs, i64 const rhs) {
                lhs += rhs;
                return lhs;
            }

            [[nodiscard]] friend bool operator==(iterator const& a, iterator const& b) {
                return a._begin == b._begin;
            }

            [[nodiscard]] friend bool operator!=(iterator const& a, iterator const& b) {
                return a._begin != b._begin;
            }

        private:
            bool has_all_components(Entity entity) {
                return (... && atl::get<Component_Container<Components>*>(_containers)->has(entity));
            }

        private:
            atl::Tuple<Component_Container<Components>*...> _containers;
            underlying_iterator_t _begin;
            underlying_iterator_t _end;
        };

    public:
        [[nodiscard]] size_type size() const {
            i64 sizes[] = {atl::get<Component_Container<Components>*>(_containers)->size()...};
            i64 min = sizes[0];
            for(i64 i = 1; i < atl::size(sizes); ++i) {
                min = math::min(min, sizes[i]);
            }
            return min;
        }

        [[nodiscard]] iterator begin() {
            auto c = find_smallest_container();
            return iterator(_containers, c->begin(), c->end());
        }

        [[nodiscard]] iterator end() {
            auto c = find_smallest_container();
            return iterator(_containers, c->end(), c->end());
        }

        template <typename... Ts>
        [[nodiscard]] decltype(auto) get(Entity const entity) {
            if constexpr (sizeof...(Ts) == 1) {
                return (..., atl::get<Component_Container<Ts>*>(_containers)->get(entity));
            } else {
                return atl::Tuple<Ts&...>(get<Ts>(entity)...);
            }
        }

        // Provides a convenient way to iterate over all entities and their components.
        // Requires a callable of form void(Components&...) or void(Entity, Components&...)
        // May be less efficient than range-based for loop or other alternatives because it prefetches all components
        //
        template <typename Callable>
        void each(Callable&& callable) {
            static_assert(atl::is_invocable<Callable, Entity, Components&...> || atl::is_invocable<Callable, Components&...>);
            auto smallest_container = find_smallest_container();
            for (Entity const entity: *smallest_container) {
                if (has_all_components(entity)) {
                    if constexpr (atl::is_invocable<Callable, Components&...>) {
                        callable(get<Components>(entity)...);
                    } else {
                        callable(entity, get<Components>(entity)...);
                    }
                }
            }
        }

    private:
        bool has_all_components(Entity entity) {
            return (... && atl::get<Component_Container<Components>*>(_containers)->has(entity));
        }

        // TODO add const support
        Component_Container_Base* find_smallest_container() {
            Component_Container_Base* conts[] = {static_cast<Component_Container_Base*>(atl::get<Component_Container<Components>*>(_containers))...};
            i64 min = conts[0]->size();
            i64 smallest_cont = 0;
            for(i64 i = 1; i < atl::size(conts); ++i) {
                i64 const size = conts[i]->size();
                if(size < min) {
                    min = size;
                    smallest_cont = i;
                }
            }
            return conts[smallest_cont];
        }

    private:
        atl::Tuple<Component_Container<Components>*...> _containers;
    };

    // Specialization of Component_View  for single component type.
    // Avoids unnecessary checks.
    template <typename Component>
    class Component_View<Component> {
        friend class ECS;

        Component_View(Component_Container<Component>* c): _container(c) {}

    public:
        using size_type = Component_Container_Base::size_type;
        using iterator = Component_Container_Base::iterator;

        [[nodiscard]] size_type size() const {
            return _container->size();
        }

        [[nodiscard]] iterator begin() {
            return _container->Component_Container_Base::begin();
        }

        [[nodiscard]] iterator end() {
            return _container->Component_Container_Base::end();
        }

        [[nodiscard]] Component& get(Entity const entity) {
            return _container->get(entity);
        }

        // Provides a convenient way to iterate over all entities and their components.
        // Requires a callable of form void(Component&) or void(Entity, Component&)
        //
        template <typename Callable>
        void each(Callable&& callable) {
            static_assert(atl::is_invocable<Callable, Entity, Component&> || atl::is_invocable<Callable, Component&>);
            for (Entity entity: *static_cast<Component_Container_Base*>(_container)) {
                if constexpr (atl::is_invocable<Callable, Component&>) {
                    callable(get(entity));
                } else {
                    callable(entity, get(entity));
                }
            }
        }

    private:
        Component_Container<Component>* _container;
    };
} // namespace anton_engine

#endif // !ENGINE_ECS_COMPONENT_VIEW_HPP_INCLUDE
