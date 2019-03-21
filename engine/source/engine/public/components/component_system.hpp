#ifndef ENGINE_COMPONENTS_COMPONENT_SYSTEM_HPP_INCLUDE
#define ENGINE_COMPONENTS_COMPONENT_SYSTEM_HPP_INCLUDE

#include "containers/vector.hpp"

#include "base_component.hpp"
#include "behaviour_component.hpp"
#include "camera.hpp"
#include "directional_light_component.hpp"
#include "engine.hpp"
#include "entity.hpp"
#include "line_component.hpp"
#include "point_light_component.hpp"
#include "renderable_component.hpp"
#include "spot_light_component.hpp"
#include "static_mesh_component.hpp"
#include "transform.hpp"

#include <map>
#include <memory>
#include <type_traits>
#include <typeindex>

class Component_System {
private:
    using Base_Component_ptr = std::unique_ptr<Base_Component>;
    using Behaviour_Component_ptr = std::unique_ptr<Behaviour_Component>;

    //public:
    //    Component_System();
    //    ~Component_System();

public:
    template <typename T, typename... Ctor_Args>
    T& add_component(Ctor_Args&&... args) {
        static_assert(!std::is_same_v<T, Renderable_Component>, "Renderable_Component is an abstract class");
        static_assert(!std::is_same_v<T, Behaviour_Component>, "Behaviour_Component is an abstract class");

        // TODO Disable double adding certain components

        if constexpr (std::is_same_v<T, Camera>) {
            T& component = camera_components.emplace_back(std::forward<Ctor_Args>(args)...);
            return component;
        } else if constexpr (std::is_same_v<T, Static_Mesh_Component>) {
            T& component = static_mesh_components.emplace_back(std::forward<Ctor_Args>(args)...);
            return component;
        } else if constexpr (std::is_same_v<T, Line_Component>) {
            T& component = static_mesh_components.emplace_back(std::forward<Ctor_Args>(args)...);
            return component;
        } else if constexpr (std::is_same_v<T, Transform>) {
            T& component = transform_components.emplace_back(std::forward<Ctor_Args>(args)...);
            return component;
        } else if constexpr (std::is_same_v<T, Point_Light_Component>) {
            T& component = point_light_components.emplace_back(std::forward<Ctor_Args>(args)...);
            return component;
        } else if constexpr (std::is_same_v<T, Directional_Light_Component>) {
            T& component = directional_light_components.emplace_back(std::forward<Ctor_Args>(args)...);
            return component;
        } else if constexpr (std::is_same_v<T, Spot_Light_Component>) {
            T& component = spot_light_components.emplace_back(std::forward<Ctor_Args>(args)...);
            return component;
        } else if constexpr (std::is_base_of_v<Behaviour_Component, T>) {
            Behaviour_Component_ptr component = std::make_unique<T>(std::forward<Ctor_Args>(args)...);
            auto type_index = std::type_index(typeid(T));
            auto iter = behaviour_components.find(type_index);
            if (iter == behaviour_components.end()) {
                auto insertion_result = behaviour_components.emplace(type_index, containers::Vector<Behaviour_Component_ptr>());
                iter = insertion_result.first;
            }
            Behaviour_Component_ptr& behaviour_component_ptr = iter->second.emplace_back(std::move(component));
            T* component_ptr = static_cast<T*>(behaviour_component_ptr.get());
            return *component_ptr;
        } else {
            Base_Component_ptr component = std::make_unique<T>(std::forward<Ctor_Args>(args)...);
            auto type_index = std::type_index(typeid(T));
            auto iter = components.find(type_index);
            if (iter == components.end()) {
                auto insertion_result = components.emplace(type_index, containers::Vector<Base_Component_ptr>());
                iter = insertion_result.first;
            }
            Base_Component_ptr& base_component_ptr = iter->second.emplace_back(std::move(component));
            T* component_ptr = static_cast<T*>(base_component_ptr.get());
            return *component_ptr;
        }
    }

    template <typename T>
    T& get_component(Entity const& entity) {
        static_assert(!std::is_same_v<T, Renderable_Component>, "Renderable_Component is an abstract class");
        static_assert(!std::is_same_v<T, Behaviour_Component>, "Behaviour_Component is an abstract class");

        if constexpr (std::is_same_v<T, Camera>) {
            return find_component(entity, camera_components);
        } else if constexpr (std::is_same_v<T, Static_Mesh_Component>) {
            return find_component(entity, static_mesh_components);
        } else if constexpr (std::is_same_v<T, Line_Component>) {
            return find_component(entity, line_components);
        } else if constexpr (std::is_same_v<T, Spot_Light_Component>) {
            return find_component(entity, spot_light_components);
        } else if constexpr (std::is_same_v<T, Point_Light_Component>) {
            return find_component(entity, point_light_components);
        } else if constexpr (std::is_same_v<T, Directional_Light_Component>) {
            return find_component(entity, directional_light_components);
        } else if constexpr (std::is_same_v<T, Transform>) {
            return find_component(entity, transform_components);
        } else if constexpr (std::is_base_of_v<Behaviour_Component, T>) {
            auto type_index = std::type_index(typeid(T));
            auto iter = behaviour_components.find(type_index);
            if (iter != behaviour_components.end()) {
                for (Behaviour_Component_ptr& component : iter->second) {
                    T* component_ptr = static_cast<T*>(component.get());
                    if (component_ptr->get_entity() == entity) {
                        return *component_ptr;
                    }
                }
            }
            throw std::runtime_error("Component not found"); // I have no clue how to solve that
        } else {
            auto type_index = std::type_index(typeid(T));
            auto iter = components.find(type_index);
            if (iter != components.end()) {
                for (Base_Component_ptr& component : iter->second) {
                    T* component_ptr = static_cast<T*>(component.get());
                    if (component_ptr->get_entity() == entity) {
                        return *component_ptr;
                    }
                }
            }
            throw std::runtime_error("Component not found"); // I have no clue how to solve that
        }
    }

    void update_behaviours();

private:
    // Exists to reduce code duplication
    template <typename T>
    auto find_component(Entity const& entity, containers::Vector<T>& component_container) -> T& {
        for (auto& component : component_container) {
            if (component.get_entity() == entity) {
                return component;
            }
        }
        throw std::runtime_error("Component not found"); // I have no clue how to solve that
    }

public:
    containers::Vector<Camera> camera_components;
    containers::Vector<Static_Mesh_Component> static_mesh_components;
    containers::Vector<Line_Component> line_components;
    containers::Vector<Directional_Light_Component> directional_light_components;
    containers::Vector<Spot_Light_Component> spot_light_components;
    containers::Vector<Point_Light_Component> point_light_components;
    containers::Vector<Transform> transform_components;

private:
    std::map<std::type_index, containers::Vector<Behaviour_Component_ptr>> behaviour_components;
    std::map<std::type_index, containers::Vector<Base_Component_ptr>> components;
};

template <typename T, typename... Ctor_Args>
T& add_component(Entity const& entity, Ctor_Args&&... args) {
    return Engine::get_component_system().add_component<T>(entity, std::forward<Ctor_Args>(args)...);
}

template <typename T>
T& get_component(Entity const& entity) {
    return Engine::get_component_system().get_component<T>(entity);
}

#endif // !ENGINE_COMPONENTS_COMPONENT_SYSTEM_HPP_INCLUDE