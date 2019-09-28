#ifndef ENGINE_ENTITY_COMPONENT_SYSTEM_HPP_INCLUDE
#define ENGINE_ENTITY_COMPONENT_SYSTEM_HPP_INCLUDE

#include <anton_stl/algorithm.hpp>
#include <anton_stl/vector.hpp>
#include <ecs/component_access.hpp>
#include <ecs/component_container.hpp>
#include <ecs/entity.hpp>
#include <engine.hpp>
#include <integer_sequence_generator.hpp>
#include <serialization/archives/binary.hpp>
#include <type_family.hpp>

#include <type_traits>

namespace anton_engine {
    class ECS {
    public:
        ECS() = default;
        ~ECS();

        // Returns: Array of entities that have Component associated with them
        template <typename Component>
        [[nodiscard]] Entity const* entities() const;

        // Returns: Array of components of type Component
        template <typename Component>
        [[nodiscard]] Component const* components() const;

        // Create entity without any attached components
        Entity create();

        // Create entity with Components... components attached.
        // Components... must be default constructible.
        template <typename... Components>
        auto create();

        void destroy(Entity);

        template <typename T, typename... Ctor_Args>
        T& add_component(Entity, Ctor_Args&&... args);

        template <typename T>
        void remove_component(Entity);

        template <typename... Ts>
        decltype(auto) get_component(Entity);

        template <typename... Ts>
        decltype(auto) try_get_component(Entity);

        template <typename... Ts>
        bool has_component(Entity);

        template <typename... Ts>
        Component_Access<Ts...> access();

        anton_stl::Vector<Entity> const& get_entities() const;

        anton_stl::Vector<Entity> const& get_entities_to_remove() const;

        void remove_requested_entities();

        friend void serialize(serialization::Binary_Output_Archive&, ECS const&);
        friend void deserialize(serialization::Binary_Input_Archive&, ECS&);

    private:
        template <typename T>
        Component_Container<T>* ensure_container();
        template <typename T>
        Component_Container<T> const* find_container() const;
        template <typename T>
        Component_Container<T>* find_container();

    private:
        using family_type = Type_Family::family_t;

        struct Components_Container_Data {
            Component_Container_Base* container = nullptr;
            void (*remove)(Component_Container_Base&, Entity);
            Type_Family::family_t family;
        };

        anton_stl::Vector<Entity> _entities;
        anton_stl::Vector<Entity> entities_to_remove;
        anton_stl::Vector<Components_Container_Data> containers;
        Integer_Sequence_Generator id_generator;
    };
} // namespace anton_engine

namespace anton_engine {
    template <typename Component>
    [[nodiscard]] inline Entity const* ECS::entities() const {
        auto const* c = find_container<Component>();
        return c ? c->entities() : nullptr;
    }

    template <typename Component>
    [[nodiscard]] inline Component const* ECS::components() const {
        auto const* c = find_container<Component>();
        return c ? c->components() : nullptr;
    }

    template <typename... Components>
    inline auto ECS::create() {
        if constexpr (sizeof...(Components) == 0) {
            return create();
        } else {
            Entity entity = create();
            return std::tuple<Entity, Components&...>{entity, add_component<Components>(entity)...};
        }
    }

    inline void ECS::destroy(Entity const entity) {
        entities_to_remove.push_back(entity);
    }

    template <typename T, typename... Ctor_Args>
    inline T& ECS::add_component(Entity const entity, Ctor_Args&&... args) {
        Component_Container<T>& components = *ensure_container<T>();
        return components.add(entity, std::forward<Ctor_Args>(args)...);
    }

    template <typename T>
    inline void ECS::remove_component(Entity const entity) {
        Component_Container<T>& components = *ensure_container<T>();
        components.remove(entity);
    }

    template <typename... Ts>
    inline decltype(auto) ECS::get_component(Entity const entity) {
        if constexpr (sizeof...(Ts) == 1) {
            Component_Container<Ts...>* components = ensure_container<Ts...>();
            return components->get(entity);
        } else {
            return std::make_tuple(get_component<Ts>(entity)...);
        }
    }

    template <typename... Ts>
    inline decltype(auto) ECS::try_get_component(Entity const entity) {
        if constexpr (sizeof...(Ts) == 1) {
            Component_Container<Ts...>* components = ensure_container<Ts...>();
            return components->try_get(entity);
        } else {
            return std::make_tuple(try_get_component<Ts>(entity)...);
        }
    }

    template <typename... Ts>
    inline bool ECS::has_component(Entity const entity) {
        static_assert(sizeof...(Ts) > 0, "Empty parameter pack");
        auto const containers = std::make_tuple(find_container<Ts>()...);
        return (... && (std::get<Component_Container<Ts>*>(containers) ? std::get<Component_Container<Ts>*>(containers)->has(entity) : false));
    }

    template <typename... Ts>
    inline Component_Access<Ts...> ECS::access() {
        return Component_Access<Ts...>(ensure_container<Ts>()...);
    }

    inline anton_stl::Vector<Entity> const& ECS::get_entities() const {
        return _entities;
    }

    inline anton_stl::Vector<Entity> const& ECS::get_entities_to_remove() const {
        return entities_to_remove;
    }

    inline void ECS::remove_requested_entities() {
        for (auto& container_data: containers) {
            for (Entity const entity: entities_to_remove) {
                if (container_data.container->has(entity)) {
                    container_data.remove(*container_data.container, entity);
                }
            }
        }

        for (Entity const entity: entities_to_remove) {
            auto iter = anton_stl::find(_entities.begin(), _entities.end(), entity);
            _entities.erase_unsorted(iter);
        }

        entities_to_remove.clear();
    }

    template <typename T>
    inline Component_Container<T>* ECS::ensure_container() {
        auto component_family = Type_Family::family_id<T>();
        for (auto& data: containers) {
            if (data.family == component_family) {
                return static_cast<Component_Container<T>*>(data.container);
            }
        }

        auto& data = containers.emplace_back();
        try {
            data.container = new Component_Container<T>();
        } catch (...) {
            containers.pop_back();
            throw;
        }
        data.family = component_family;
        data.remove = [](Component_Container_Base& container, Entity const entity) { static_cast<Component_Container<T>&>(container).remove(entity); };
        return static_cast<Component_Container<T>*>(data.container);
    }

    template <typename T>
    inline Component_Container<T> const* ECS::find_container() const {
        auto component_family = Type_Family::family_id<T>();
        for (auto& data: containers) {
            if (data.family == component_family) {
                return static_cast<Component_Container<T> const*>(data.container);
            }
        }
        return nullptr;
    }

    template <typename T>
    inline Component_Container<T>* ECS::find_container() {
        auto component_family = Type_Family::family_id<T>();
        for (auto& data: containers) {
            if (data.family == component_family) {
                return static_cast<Component_Container<T>*>(data.container);
            }
        }
        return nullptr;
    }
} // namespace anton_engine

#endif // !ENGINE_ENTITY_COMPONENT_SYSTEM_HPP_INCLUDE
