#ifndef ENGINE_COMPONENTS_COMPONENT_SYSTEM_HPP_INCLUDE
#define ENGINE_COMPONENTS_COMPONENT_SYSTEM_HPP_INCLUDE

#include "containers/swapping_pool.hpp"

#include "behaviour_component.hpp"
#include "camera.hpp"
#include "directional_light_component.hpp"
#include "point_light_component.hpp"
#include "renderable_component.hpp"
#include "spot_light_component.hpp"
#include "static_mesh_component.hpp"
#include "transform.hpp"

#include <any>
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
            T& component = camera_components.add(std::forward<Ctor_Args>(args)...);
            return component;
        } else if constexpr (std::is_same_v<T, Static_Mesh_Component>) {
            T& component = static_mesh_components.add(std::forward<Ctor_Args>(args)...);
            return component;
        } else if constexpr (std::is_same_v<T, Transform>) {
            T& component = transform_components.add(std::forward<Ctor_Args>(args)...);
            return component;
        } else if constexpr (std::is_same_v<T, Point_Light_Component>) {
            T& component = point_light_components.add(std::forward<Ctor_Args>(args)...);
            return component;
        } else if constexpr (std::is_same_v<T, Directional_Light_Component>) {
            T& component = directional_light_components.add(std::forward<Ctor_Args>(args)...);
            return component;
        } else if constexpr (std::is_same_v<T, Spot_Light_Component>) {
            T& component = spot_light_components.add(std::forward<Ctor_Args>(args)...);
            return component;
        } else if constexpr (std::is_base_of_v<Behaviour_Component, T>) {
            Behaviour_Component_ptr component = std::make_unique<T>(std::forward<Ctor_Args>(args)...);
            auto type_index = std::type_index(typeid(T));
            auto iter = behaviour_components.find(type_index);
            if (iter == behaviour_components.end()) {
                auto insertion_result = behaviour_components.emplace(type_index, Swapping_Pool<Behaviour_Component_ptr>());
                iter = insertion_result.first;
            }
            Behaviour_Component_ptr& behaviour_component_ptr = iter->second.add(std::move(component));
            T* component_ptr = static_cast<T*>(behaviour_component_ptr.get());
            return *component_ptr;
        } else {
            Base_Component_ptr component = std::make_unique<T>(std::forward<Ctor_Args>(args)...);
            auto type_index = std::type_index(typeid(T));
            auto iter = components.find(type_index);
            if (iter == components.end()) {
                auto insertion_result = components.emplace(type_index, Swapping_Pool<Base_Component_ptr>());
                iter = insertion_result.first;
            }
            Base_Component_ptr& base_component_ptr = iter->second.add(std::move(component));
            T* component_ptr = static_cast<T*>(base_component_ptr.get());
            return *component_ptr;
        }
    }

    template <typename T>
    T& get_component(Game_Object const& go) {
        static_assert(!std::is_same_v<T, Renderable_Component>, "Renderable_Component is an abstract class");
        static_assert(!std::is_same_v<T, Behaviour_Component>, "Behaviour_Component is an abstract class");

        if constexpr (std::is_same_v<T, Camera>) {
            return find_component(go, camera_components);
        } else if constexpr (std::is_same_v<T, Static_Mesh_Component>) {
            return find_component(go, static_mesh_components);
        } else if constexpr (std::is_same_v<T, Spot_Light_Component>) {
            return find_component(go, spot_light_components);
        } else if constexpr (std::is_same_v<T, Point_Light_Component>) {
            return find_component(go, point_light_components);
        } else if constexpr (std::is_same_v<T, Directional_Light_Component>) {
            return find_component(go, directional_light_components);
        } else if constexpr (std::is_same_v<T, Transform>) {
            return find_component(go, transform_components);
        } else if constexpr (std::is_base_of_v<Behaviour_Component, T>) {
            auto type_index = std::type_index(typeid(T));
            auto iter = behaviour_components.find(type_index);
            if (iter != behaviour_components.end()) {
                for (Behaviour_Component_ptr& component : iter->second) {
                    T* component_ptr = static_cast<T*>(component.get());
                    if (component_ptr->game_object == go) {
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
                    if (component_ptr->game_object == go) {
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
    auto find_component(Game_Object const& go, Swapping_Pool<T>& component_container) -> T& {
        for (auto& component : component_container) {
            if (component.game_object == go) {
                return component;
            }
        }
        throw std::runtime_error("Component not found"); // I have no clue how to solve that
    }

public:
    Swapping_Pool<Camera> camera_components;
    Swapping_Pool<Static_Mesh_Component> static_mesh_components;
    Swapping_Pool<Directional_Light_Component> directional_light_components;
    Swapping_Pool<Spot_Light_Component> spot_light_components;
    Swapping_Pool<Point_Light_Component> point_light_components;
    Swapping_Pool<Transform> transform_components;

private:
    std::map<std::type_index, Swapping_Pool<Behaviour_Component_ptr>> behaviour_components;
    std::map<std::type_index, Swapping_Pool<Base_Component_ptr>> components;
};

#endif // !ENGINE_COMPONENTS_COMPONENT_SYSTEM_HPP_INCLUDE