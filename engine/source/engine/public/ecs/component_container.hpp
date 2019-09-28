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

        [[nodiscard]] Entity* data();
        [[nodiscard]] Entity const* data() const;

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
        anton_stl::Vector<size_type> indirect;
        anton_stl::Vector<Entity> entities;
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

        [[nodiscard]] Component const* raw() const {
            if constexpr (anton_stl::is_empty<Component>) {
                return &components;
            } else {
                return components.data();
            }
        }

        [[nodiscard]] Component* raw() {
            if constexpr (anton_stl::is_empty<Component>) {
                return &components;
            } else {
                return components.data();
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
                return components;
            } else {
                components.emplace_back(std::forward<Args>(args)...);
                add_entity(entity);
                return components.back();
            }
        }

        void remove(Entity const entity) {
            if constexpr (!anton_stl::is_empty<Component>) {
                components.erase_unsorted(get_component_index(entity));
            }

            remove_entity(entity);
        }

        [[nodiscard]] Component& get(Entity const entity) {
            ANTON_ASSERT(has(entity), "Attempting to get component of an entity that has not been registered");

            if constexpr (anton_stl::is_empty<Component>) {
                return components;
            } else {
                return components[get_component_index(entity)];
            }
        }

        [[nodiscard]] Component* try_get(Entity const entity) {
            if constexpr (anton_stl::is_empty<Component>) {
                return has(entity) ? &components : nullptr;
            } else {
                return has(entity) ? components.data() + get_component_index(entity) : nullptr;
            }
        }

    private:
        anton_stl::conditional<anton_stl::is_empty<Component>, Component, anton_stl::Vector<Component>> components;
    };
} // namespace anton_engine

namespace anton_engine {
    inline void serialize(serialization::Binary_Output_Archive& archive, Component_Container_Base const& container) {
        serialize(archive, container.entities);
    }

    inline void deserialize(serialization::Binary_Input_Archive& archive, Component_Container_Base& container) {
        deserialize(archive, container.entities);
        for (int64_t i = 0; i < container.entities.size(); i += 1) {
            auto index = container.indirect_index(container.entities[i]);
            container.ensure(index);
            container.indirect[index] = i;
        }
    }
} // namespace anton_engine

#include <ecs/component_container.tpp>

#endif // !ENGINE_ECS_COMPONENT_CONTAINER_HPP_INCLUDE
