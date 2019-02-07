#ifndef ENGINE_GAME_OBJECT_HPP_INCLUDE
#define ENGINE_GAME_OBJECT_HPP_INCLUDE

#include "components/component_system.hpp"
#include "components/transform.hpp"
#include "engine.hpp"
#include "object.hpp"

class Game_Object : public Object {
public:
    Transform transform; // TODO move out of game object to component system

public:
    Game_Object(); // TODO temporary, remove afterwards

    template <typename T>
    T& get_component() {
        return Engine::get_component_system().get_component<T>(*this);
    }

    template <typename T, typename... Ctor_Args>
    T& add_component(Ctor_Args&&... args) {
        return Engine::get_component_system().add_component<T>(*this, std::forward<Ctor_Args>(args)...);
    }
};

#endif // !ENGINE_GAME_OBJECT_HPP_INCLUDE