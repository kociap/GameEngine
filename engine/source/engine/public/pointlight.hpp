#ifndef ENGINE_POINT_LIGHT_HPP_INCLUDE
#define ENGINE_POINT_LIGHT_HPP_INCLUDE

#include "color.hpp"
#include "math/vector3.hpp"

class Point_light {
public:
    Vector3 position;
    Color color;
};

#endif // !ENGINE_POINT_LIGHT_HPP_INCLUDE