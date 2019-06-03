#ifndef PHYSICS_PHYSICS_HPP_INCLUDE
#define PHYSICS_PHYSICS_HPP_INCLUDE

#include "math/matrix4.hpp"
#include "math/vector2.hpp"
#include "math/vector3.hpp"
#include "ray.hpp"
#include <cstdint>

namespace physics {
    using Point = Vector3;

    class Raycast_Hit {
    public:
        Point hit_point;
        Vector3 barycentric_coordinates;
        float distance = 0;
    };

    bool raycast_test(Ray);
    Raycast_Hit raycast(Ray);
} // namespace physics

#endif // !PHYSICS_PHYSICS_HPP_INCLUDE
