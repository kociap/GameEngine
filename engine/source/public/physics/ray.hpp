#ifndef PHYSICS_RAY_HPP_INCLUDE
#define PHYSICS_RAY_HPP_INCLUDE

#include <core/types.hpp>
#include <core/math/matrix4.hpp>
#include <core/math/vector2.hpp>
#include <core/math/vector3.hpp>

namespace anton_engine {
    class Ray {
    public:
        Vector3 origin;
        Vector3 direction;
    };

    Ray screen_to_ray(Matrix4 inv_view, Matrix4 inv_projection, i32 screen_width, i32 screen_height, Vector2 point);
    Ray screen_to_ray(Matrix4 inv_view_projection, i32 screen_width, i32 screen_height, Vector2 point);
} // namespace anton_engine

#endif // !PHYSICS_RAY_HPP_INCLUDE
