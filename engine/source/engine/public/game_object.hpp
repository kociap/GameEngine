#ifndef ENGINE_GAME_OBJECT_HPP_INCLUDE
#define ENGINE_GAME_OBJECT_HPP_INCLUDE

#include "components/base_component.hpp"
#include "components/camera.hpp"
#include "components/transform.hpp"
#include "object.hpp"
#include <cstdint>
#include <map>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <typeindex>
#include <vector>

class Game_Object : public Object {
public:
    using Base_Component_ptr = std::unique_ptr<Base_Component>;

private:
    std::map<std::type_index, std::vector<Base_Component_ptr>> components;

public:
    Transform transform;

public:
    Game_Object(); // TODO temporary, remove afterwards

    void update();

    template <typename T>
    T& get_component() {
        auto iter = components.find(std::type_index(typeid(T)));
		if (iter == components.end() || iter->second.size() == 0) {
            throw std::runtime_error("Component not attached");
		}
		Base_Component_ptr& base_component_ptr = iter->second[0];
		T* component_ptr = static_cast<T*>(base_component_ptr.get());
		return *component_ptr;
    }

    template <typename T, typename... CtorArgs>
    T& add_component(CtorArgs&&... args) {
        // TODO Disable adding Transform and double adding certain components

        auto type_index = std::type_index(typeid(T));
        Base_Component_ptr component = std::make_unique<T>(*this, std::forward<CtorArgs>(args)...);
        auto iter = components.find(type_index);
        if (iter == components.end()) {
            auto insertion_result = components.emplace(type_index, std::vector<Base_Component_ptr>());
            iter = insertion_result.first;
        }
        iter->second.push_back(std::move(component));
        Base_Component_ptr& base_component_ptr = iter->second.back();
        T* component_ptr = static_cast<T*>(base_component_ptr.get());
        return *component_ptr;
    }
};

#endif // !ENGINE_GAME_OBJECT_HPP_INCLUDE