#ifndef ENGINE_ENTITY_COMPONENT_SYSTEM_HPP_INCLUDE
#define ENGINE_ENTITY_COMPONENT_SYSTEM_HPP_INCLUDE

#include <containers/vector.hpp>
#include <ecs/component_access.hpp>
#include <ecs/component_container.hpp>
#include <ecs/entity.hpp>
#include <engine.hpp>
#include <integer_sequence_generator.hpp>
#include <iterators.hpp>
#include <serialization/archives/binary.hpp>
#include <type_family.hpp>

#include <type_traits>

class ECS;

namespace serialization {
    void serialize(Binary_Output_Archive&, ECS const&);
    void deserialize(Binary_Input_Archive&, ECS&);
} // namespace serialization

class ECS {
private:
    using family_type = Type_Family::family_t;

    struct Components_Container_Data {
        Component_Container_Base* container = nullptr;
        Type_Family::family_t family;
    };

public:
    using iterator = iterators::iterator<ECS>;
    using const_iterator = iterators::const_iterator<ECS>;

    friend void serialization::serialize(serialization::Binary_Output_Archive&, ECS const&);
    friend void serialization::deserialize(serialization::Binary_Input_Archive&, ECS&);

    ECS() = default;
    ~ECS();

    template <typename Component>
    [[nodiscard]] Component const* data() const {
        auto const* c = find_container<Component>();
        return c ? c->data() : nullptr;
    }

    template <typename Component>
    [[nodiscard]] Component const* raw() const {
        auto const* c = find_container<Component>();
        return c ? c->raw() : nullptr;
    }

    template <typename... Components>
    Entity create() {
        static Integer_Sequence_Generator id_generator;
        Entity entity = entities.emplace_back(id_generator.next());
        if constexpr (sizeof...(Components) > 0) {
            (..., add_component<Components>(entity));
        }

        return entity;
    }

    template <typename T, typename... Ctor_Args>
    T& add_component(Entity const entity, Ctor_Args&&... args) {
        Component_Container<T>& components = *ensure_container<T>();
        return components.add(entity, std::forward<Ctor_Args>(args)...);
    }

    template <typename T>
    void remove_component(Entity const entity) {
        Component_Container<T>& components = *ensure_container<T>();
        components.remove(entity);
    }

    template <typename... Ts>
    decltype(auto) get_component(Entity const entity) {
        if constexpr (sizeof...(Ts) == 1) {
            Component_Container<Ts...>& components = *ensure_container<Ts...>();
            return components.get(entity);
        } else {
            return std::make_tuple(get_component<Ts>(entity)...);
        }
    }

    template <typename... Ts>
    void has_component(Entity const entity) {
        static_assert(sizeof...(Ts) > 0, "Empty parameter pack");
        auto const containers = std::make_tuple(find_container<Ts>()...);
        return (... && (std::get<Component_Container<Ts>*>(containers) ? std::get<Component_Container<Ts>*>(containers)->has(entity) : false));
    }

    template <typename... Ts>
    Component_Access<Ts...> access() {
        return Component_Access<Ts...>(ensure_container<Ts>()...);
    }

private:
    template <typename T>
    Component_Container<T>* ensure_container() {
        auto component_family = Type_Family::family_id<T>();
        for (auto& data: containers) {
            if (data.family == component_family) {
                return static_cast<Component_Container<T>*>(data.container);
            }
        }

        auto& data = containers.emplace_back();
        // TODO
        try {
            data.container = new Component_Container<T>();
        } catch (...) {
            containers.pop_back();
            throw;
        }
        data.family = component_family;
        return static_cast<Component_Container<T>*>(data.container);
    }

    template <typename T>
    Component_Container<T> const* find_container() const {
        auto component_family = Type_Family::family_id<T>();
        for (auto& data: containers) {
            if (data.family == component_family) {
                return static_cast<Component_Container<T> const*>(data.container);
            }
        }
        return nullptr;
    }

    template <typename T>
    Component_Container<T>* find_container() {
        return const_cast<Component_Container<T>*>(const_cast<ECS const&>(*this).template find_container<T>());
    }

    void serialize_component_container(Type_Family::family_t identifier, serialization::Binary_Output_Archive&, Component_Container_Base const*) const;
    void deserialize_component_container(Type_Family::family_t identifier, serialization::Binary_Input_Archive&, Component_Container_Base*&);

private:
    containers::Vector<Entity> entities;
    containers::Vector<Components_Container_Data> containers;
};

#endif // !ENGINE_ENTITY_COMPONENT_SYSTEM_HPP_INCLUDE
