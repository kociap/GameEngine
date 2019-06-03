#ifndef PHYSICS_COLLISIONS_HPP_INCLUDE
#define PHYSICS_COLLISIONS_HPP_INCLUDE

#include "math/matrix4.hpp"
#include "math/vector3.hpp"
#include "obb.hpp"
#include "physics.hpp"

class Mesh;

namespace physics {
    bool intersect_ray_triangle(Ray, Point, Point, Point, Raycast_Hit& out);
    bool test_ray_obb(Ray, OBB);
    bool intersect_ray_obb(Ray, OBB, Raycast_Hit& out);
    bool test_ray_mesh(Ray, Mesh const&);
    bool intersect_ray_mesh(Ray, Mesh const&, Matrix4 model_transform, Raycast_Hit&);
} // namespace physics

#endif // !PHYSICS_COLLISIONS_HPP_INCLUDE
