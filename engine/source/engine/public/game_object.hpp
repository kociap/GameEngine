#ifndef ENGINE_GAME_OBJECT_HPP_INCLUDE
#define ENGINE_GAME_OBJECT_HPP_INCLUDE

#include "components/component_system.hpp"
#include "engine.hpp"
#include "object.hpp"

class Game_Object : public Object {
    friend Game_Object& instantiate();
    friend void destroy(Game_Object&);

public:
    Game_Object();

public:
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