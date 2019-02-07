#ifndef ENGINE_COMPONENTS_COMPONENT_SYSTEM_HPP_INCLUDE
#define ENGINE_COMPONENTS_COMPONENT_SYSTEM_HPP_INCLUDE

#include "containers/swapping_pool.hpp"

#include "camera.hpp"
#include "renderable_component.hpp"
#include "scriptable_component.hpp"
#include "static_mesh_component.hpp"
#include "directional_light_component.hpp"
#include "spot_light_component.hpp"
#include "point_light_component.hpp"

#include <map>
#include <memory>
#include <typeindex>
#include <type_traits>

class Component_System {
private:
    using Base_Component_ptr = std::unique_ptr<Base_Component>;

public:
    template <typename T, typename... Ctor_Args>
    T& add_component(Ctor_Args&&... args) {
		// Disable adding Transform because objects have one by default
        static_assert(!std::is_same_v<T, Transform>, "Transform already exists on a game_object"); 
        static_assert(!std::is_same_v<T, Renderable_Component>, "Renderable_Component is an abstract class");
        static_assert(!std::is_same_v<T, Scriptable_Component>, "Renderable_Component is an abstract class");

        // TODO Disable double adding certain components

        if constexpr (std::is_same_v<T, Camera>) {
            T& component = camera_components.add(std::forward<Ctor_Args>(args)...);
            return component;
        } else if constexpr (std::is_same_v<T, Static_Mesh_Component>) {
            T& component = static_mesh_components.add(std::forward<Ctor_Args>(args)...);
            return component;
        } else {
            Base_Component_ptr component = std::make_unique<T>(std::forward<Ctor_Args>(args)...);
            auto type_index = std::type_index(typeid(T));
            auto iter = components.find(type_index);
            if (iter == components.end()) {
                auto insertion_result = components.emplace(type_index, Swapping_Pool<Base_Component_ptr>());
                iter = insertion_result.first;
            }
            iter->second.push_back(std::move(component));
            Base_Component_ptr& base_component_ptr = iter->second.back();
            T* component_ptr = static_cast<T*>(base_component_ptr.get());
        }
    }

    template <typename T>
    T& get_component(Game_Object const& go) {
        static_assert(!std::is_same_v<T, Renderable_Component>, "Renderable_Component is an abstract class");
        static_assert(!std::is_same_v<T, Scriptable_Component>, "Renderable_Component is an abstract class");

        if constexpr (std::is_same_v<T, Camera>) {
			return find_component(go, camera_components);
        } else if constexpr (std::is_same_v<T, Static_Mesh_Component>) {
            return find_component(go, static_mesh_components);
		} else if constexpr(std::is_same_v<T, Spot_Light_Component>) {
            return find_component(go, spot_light_components);
        } else if constexpr (std::is_same_v<T, Point_Light_Component>) {
            return find_component(go, point_light_components);
        } else if constexpr (std::is_same_v<T, Directional_Light_Component>) {
            return find_component(go, directional_light_components);
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
    std::map<std::type_index, Swapping_Pool<Base_Component_ptr>> components;
};

#endif // !ENGINE_COMPONENTS_COMPONENT_SYSTEM_HPP_INCLUDE