#ifndef ENGINE_GAME_OBJECT_HPP_INCLUDE
#define ENGINE_GAME_OBJECT_HPP_INCLUDE

#include "components/base_component.hpp"
#include "object.hpp"
#include <cstdint>
#include <map>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <typeindex>
#include <vector>

class Game_Object : public Object {
private:
    using Base_Component_ptr = std::unique_ptr<component::Base_Component>;

public:
    void update();

    template <typename T>
    T& get_component() {
        auto iter = components.find(std::type_index(typeid(T));
		if (iter == components.end() || iter->second.size() == 0) {
            throw std::runtime_error("Component not attached");
		}
		T* component_ptr = static_cast<T*>(iter->second[0].get());
		return *component_ptr;
    }

    template <typename T, typename... CtorArgs>
    void add_component(CtorArgs&&... args) {
        auto type_index = std::type_index(typeid(T));
        std::unique_ptr<Base_component> component = std::make_unique(std::forward<std::decay_t<CtorArgs>>(args)...);
        auto iter = components.find(type_index);
        if (iter == components.end()) {
            components.insert(std::vector<Base_Component_ptr>{component}));
        } else {
            iter->second.push_back(std::move(component));
        }
    }

private:
    std::map<std::type_index, std::vector<Base_Component_ptr>> components;
};

#endif // !ENGINE_GAME_OBJECT_HPP_INCLUDE