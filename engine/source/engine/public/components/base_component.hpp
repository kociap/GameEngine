#ifndef ENGINE_COMPONENTS_BASE_COMPONENT_HPP_INCLUDE
#define ENGINE_COMPONENTS_BASE_COMPONENT_HPP_INCLUDE

#include "object.hpp"
#include <cstdint>

class Game_Object;

namespace component {
    class Base_Component : public Object {
    public:
        Base_Component() = default;
        virtual ~Base_Component();

        virtual void update();

    private:
        uint64_t id;
        Game_Object* game_object;
    };
} // namespace component

#endif // !ENGINE_COMPONENTS_BASE_COMPONENT_HPP_INCLUDE