#ifndef PHYSICS_COLLISIONS_HPP_INCLUDE
#define PHYSICS_COLLISIONS_HPP_INCLUDE

#include "math/matrix4.hpp"
#include "math/vector3.hpp"
#include "physics.hpp"

class Mesh;

namespace physics {
    bool intersect_ray_triangle(Ray, Point, Point, Point, Raycast_Hit&);
    // Raycast_Hit intersect_ray_triangle(Ray, Point, Point, Point, Vector3 triangle_normal);
    bool test_ray_mesh(Ray, Mesh const&);
    bool intersect_ray_mesh(Ray, Mesh const&, Matrix4 model_transform, Raycast_Hit&);
} // namespace physics

#endif // !PHYSICS_COLLISIONS_HPP_INCLUDE
