#ifndef ENGINE_ECS_COMPONENT_CONTAINER_HPP_INCLUDE
#define ENGINE_ECS_COMPONENT_CONTAINER_HPP_INCLUDE

#include <anton_assert.hpp>
#include <anton_stl/type_traits.hpp>
#include <anton_stl/vector.hpp>
#include <ecs/component_container_iterator.hpp>
#include <ecs/entity.hpp>
#include <serialization/archives/binary.hpp>

namespace anton_engine {
    class Component_Container_Base {
    public:
        using size_type = anton_stl::Vector<Entity>::size_type;
        using iterator = anton_stl::Vector<Entity>::iterator;
        using const_iterator = anton_stl::Vector<Entity>::const_iterator;

    protected:
        constexpr static size_type npos = static_cast<size_type>(-1);

    public:
        virtual ~Component_Container_Base() = default;

        [[nodiscard]] Entity* entities();
        [[nodiscard]] Entity const* entities() const;
        [[nodiscard]] iterator begin();
        [[nodiscard]] iterator end();
        [[nodiscard]] bool has(Entity entity);
        [[nodiscard]] size_type size() const;

        friend void serialize(serialization::Binary_Output_Archive&, Component_Container_Base const&);
        friend void deserialize(serialization::Binary_Input_Archive&, Component_Container_Base&);

    protected:
        void add_entity(Entity entity);
        [[nodiscard]] size_type get_component_index(Entity entity);
        void remove_entity(Entity entity);

    private:
        [[nodiscard]] size_type indirect_index(Entity entity);
        void ensure(size_type index);

    private:
        // Indices into entities vector
        anton_stl::Vector<size_type> _indirect;
        anton_stl::Vector<Entity> _entities;
    };

    template <typename Component>
    class Component_Container: public Component_Container_Base {
    private:
        using base_t = Component_Container_Base;

    public:
        using iterator = Component_Container_Iterator<Component>;

        static void serialize(serialization::Binary_Output_Archive& archive, Component_Container_Base const*);
        static void deserialize(serialization::Binary_Input_Archive& archive, Component_Container_Base*&);

        virtual ~Component_Container() = default;

        [[nodiscard]] Component const* components() const {
            if constexpr (anton_stl::is_empty<Component>) {
                return &_components;
            } else {
                return _components.data();
            }
        }

        [[nodiscard]] Component* components() {
            if constexpr (anton_stl::is_empty<Component>) {
                return &_components;
            } else {
                return _components.data();
            }
        }

        [[nodiscard]] iterator begin() {
            return {raw(), 0};
        }

        [[nodiscard]] iterator end() {
            return {raw(), size()};
        }

        template <typename... Args>
        Component& add(Entity const entity, Args&&... args) {
            ANTON_ASSERT(!has(entity), "Attempting to add duplicate entity");
            if constexpr (anton_stl::is_empty<Component>) {
                add_entity(entity);
                return _components;
            } else {
                _components.emplace_back(std::forward<Args>(args)...);
                add_entity(entity);
                return _components.back();
            }
        }

        void remove(Entity const entity) {
            if constexpr (!anton_stl::is_empty<Component>) {
                _components.erase_unsorted(get_component_index(entity));
            }

            remove_entity(entity);
        }

        [[nodiscard]] Component& get(Entity const entity) {
            ANTON_ASSERT(has(entity), "Attempting to get component of an entity that has not been registered");
            if constexpr (anton_stl::is_empty<Component>) {
                return _components;
            } else {
                return _components[get_component_index(entity)];
            }
        }

        [[nodiscard]] Component* try_get(Entity const entity) {
            if constexpr (anton_stl::is_empty<Component>) {
                return has(entity) ? &_components : nullptr;
            } else {
                return has(entity) ? _components.data() + get_component_index(entity) : nullptr;
            }
        }

    private:
        anton_stl::conditional<anton_stl::is_empty<Component>, Component, anton_stl::Vector<Component>> _components;
    };
} // namespace anton_engine

namespace anton_engine {
    inline Entity* Component_Container_Base::entities() {
        return _entities.data();
    }

    inline Entity const* Component_Container_Base::entities() const {
        return _entities.data();
    }

    inline Component_Container_Base::iterator Component_Container_Base::begin() {
        return _entities.begin();
    }

    inline Component_Container_Base::iterator Component_Container_Base::end() {
        return _entities.end();
    }

    inline bool Component_Container_Base::has(Entity const entity) {
        auto index = indirect_index(entity);
        return index < _indirect.size() && _indirect[index] != npos;
    }

    inline Component_Container_Base::size_type Component_Container_Base::size() const {
        return _entities.size();
    }

    inline void Component_Container_Base::add_entity(Entity const entity) {
        ANTON_ASSERT(!has(entity), "Entity has already been registered");
        _entities.emplace_back(entity);
        auto index = indirect_index(entity);
        ensure(index);
        _indirect[index] = _entities.size() - 1;
    }

    inline Component_Container_Base::size_type Component_Container_Base::get_component_index(Entity const entity) {
        ANTON_ASSERT(has(entity), "Attempting to get index of an entity that has not been registered");
        return _indirect[indirect_index(entity)];
    }

    inline void Component_Container_Base::remove_entity(Entity const entity) {
        ANTON_ASSERT(has(entity), "Attempting to remove entity that has not been registered");
        auto index = indirect_index(entity);
        auto back_index = indirect_index(_entities.back());
        _entities.erase_unsorted(index);
        _indirect[back_index] = _indirect[index];
        _indirect[index] = npos;
    }

    inline Component_Container_Base::size_type Component_Container_Base::indirect_index(Entity const entity) {
        return entity.id;
    }

    inline void Component_Container_Base::ensure(size_type index) {
        if (_indirect.size() <= index) {
            _indirect.resize(index + 1, npos);
        }
    }

    inline void serialize(serialization::Binary_Output_Archive& archive, Component_Container_Base const& container) {
        serialize(archive, container._entities);
    }

    inline void deserialize(serialization::Binary_Input_Archive& archive, Component_Container_Base& container) {
        deserialize(archive, container._entities);
        for (int64_t i = 0; i < container._entities.size(); i += 1) {
            auto index = container.indirect_index(container._entities[i]);
            container.ensure(index);
            container._indirect[index] = i;
        }
    }
} // namespace anton_engine

#include <ecs/component_container.tpp>

#endif // !ENGINE_ECS_COMPONENT_CONTAINER_HPP_INCLUDE
