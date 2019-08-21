#ifndef PHYSICS_RAY_HPP_INCLUDE
#define PHYSICS_RAY_HPP_INCLUDE

#include <cstdint>
#include <math/matrix4.hpp>
#include <math/vector2.hpp>
#include <math/vector3.hpp>
#include <utility>

class Ray {
public:
    Vector3 origin;
    Vector3 direction;
};

Ray screen_to_ray(Matrix4 inv_view, Matrix4 inv_projection, int32_t screen_width, int32_t screen_height, Vector2 point);
Ray screen_to_ray(Matrix4 inv_view_projection, int32_t screen_width, int32_t screen_height, Vector2 point);

#endif // !PHYSICS_RAY_HPP_INCLUDE
