#ifndef PHYSICS_RAY_HPP_INCLUDE
#define PHYSICS_RAY_HPP_INCLUDE

#include "math/matrix4.hpp"
#include "math/vector2.hpp"
#include "math/vector3.hpp"
#include <cstdint>
#include <utility>

class Ray {
public:
    Vector3 origin;
    Vector3 direction;
};

Ray screen_to_ray(Matrix4 inv_view, Matrix4 inv_projection, uint32_t screen_width, uint32_t screen_height, Vector2 point);
Ray screen_to_ray(Matrix4 inv_view_projection, uint32_t screen_width, uint32_t screen_height, Vector2 point);

#endif // !PHYSICS_RAY_HPP_INCLUDE