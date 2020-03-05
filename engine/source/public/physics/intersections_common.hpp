#ifndef PHYSICS_INTERSECTIONS_COMMON_HPP_INCLUDE
#define PHYSICS_INTERSECTIONS_COMMON_HPP_INCLUDE

#include <core/math/vector3.hpp>

namespace anton_engine {
    class Raycast_Hit {
    public:
        Vector3 hit_point;
        Vector3 barycentric_coordinates;
        float distance = 0;
    };
} // namespace anton_engine

#endif // !PHYSICS_INTERSECTIONS_COMMON_HPP_INCLUDE
