#ifndef PHYSICS_LINE_HPP_INCLUDE
#define PHYSICS_LINE_HPP_INCLUDE

#include <core/math/vector2.hpp>
#include <core/math/vector3.hpp>

namespace anton_engine {
    class Line {
    public:
        Vector3 origin;
        Vector3 direction;
    };

    // x is coefficient of the first line, y of the second one
    Vector2 closest_points_on_lines(Line, Line);
} // namespace anton_engine

#endif // !PHYSICS_LINE_HPP_INCLUDE
