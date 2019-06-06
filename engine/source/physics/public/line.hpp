#ifndef PHYSICS_LINE_HPP_INCLUDE
#define PHYSICS_LINE_HPP_INCLUDE

#include "math/vector2.hpp"
#include "math/vector3.hpp"
#include <utility>

class Line {
public:
    Vector3 origin;
    Vector3 direction;
};

// x is coefficient of the first line, y of the second one
Vector2 closest_points_on_lines(Line, Line);

#endif // !PHYSICS_LINE_HPP_INCLUDE
