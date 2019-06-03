#ifndef PHYSICS_INTERSECTION_TESTS_HPP_INCLUDE
#define PHYSICS_INTERSECTION_TESTS_HPP_INCLUDE

#include "math/matrix4.hpp"
#include "math/vector3.hpp"
#include "obb.hpp"
#include "ray.hpp"

class Mesh;

class Raycast_Hit {
public:
    Vector3 hit_point;
    Vector3 barycentric_coordinates;
    float distance = 0;
};

bool intersect_ray_triangle(Ray, Vector3, Vector3, Vector3, Raycast_Hit& out);
bool test_ray_obb(Ray, OBB);
bool intersect_ray_obb(Ray, OBB, Raycast_Hit& out);
bool test_ray_mesh(Ray, Mesh const&);
bool intersect_ray_mesh(Ray, Mesh const&, Matrix4 model_transform, Raycast_Hit&);

#endif // !PHYSICS_INTERSECTION_TESTS_HPP_INCLUDE
