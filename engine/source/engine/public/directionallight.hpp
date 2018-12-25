#ifndef ENGINE_DIRECITONAL_LIGHT_HPP_INCLUDE
#define ENGINE_DIRECITONAL_LIGHT_HPP_INCLUDE

#include "math/vector3.hpp"
#include "color.hpp"

class Directional_light {
public:
    Vector3 position;
    Vector3 direction;
    Color color;
};

#endif // !ENGINE_DIRECITONAL_LIGHT_HPP_INCLUDE