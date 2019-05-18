#ifndef PHYSICS_PHYSICS_HPP_INCLUDE
#define PHYSICS_PHYSICS_HPP_INCLUDE

#include "math/matrix4.hpp"
#include "math/vector2.hpp"
#include "math/vector3.hpp"
#include <cstdint>

namespace physics {
    using Point = Vector3;

    class Ray {
    public:
        Vector3 origin;
        Vector3 direction;
    };

    class Raycast_Hit {
    public:
        Point hit_point;
        Vector3 barycentric_coordinates;
        float distance = 0;
    };

    bool raycast_test(Ray);
    Raycast_Hit raycast(Ray);
} // namespace physics

physics::Ray screen_to_ray(Matrix4 inv_view, Matrix4 inv_projection, uint32_t screen_width, uint32_t screen_height, Vector2 point);
physics::Ray screen_to_ray(Matrix4 inv_view_projection, uint32_t screen_width, uint32_t screen_height, Vector2 point);

#endif // !PHYSICS_PHYSICS_HPP_INCLUDE
