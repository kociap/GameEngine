#ifndef ENGINE_COMPONENTS_BASE_COMPONENT_HPP_INCLUDE
#define ENGINE_COMPONENTS_BASE_COMPONENT_HPP_INCLUDE

#include "object.hpp"
#include <cstdint>

class Game_Object;
class Transform;

class Base_Component : public Object {
public:
    Base_Component() = delete;
    Base_Component(Game_Object& game_object);
    Base_Component(Base_Component const&) = delete;
    Base_Component(Base_Component&&) = default;
    Base_Component& operator=(Base_Component const&) = delete;
    Base_Component& operator=(Base_Component&&) = delete;
    virtual ~Base_Component();

    Transform& get_transform();

public:
    Game_Object& game_object;
};

#endif // !ENGINE_COMPONENTS_BASE_COMPONENT_HPP_INCLUDE