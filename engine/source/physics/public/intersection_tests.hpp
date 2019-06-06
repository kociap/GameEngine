#ifndef PHYSICS_INTERSECTION_TESTS_HPP_INCLUDE
#define PHYSICS_INTERSECTION_TESTS_HPP_INCLUDE

#include "line.hpp"
#include "math/matrix4.hpp"
#include "math/vector3.hpp"
#include "obb.hpp"
#include "ray.hpp"
#include <optional>

class Mesh;

class Raycast_Hit {
public:
    Vector3 hit_point;
    Vector3 barycentric_coordinates;
    float distance = 0;
};

class Linecast_Hit {
public:
    Vector3 hit_point;
    float distance = 0;
};

std::optional<Raycast_Hit> intersect_ray_triangle(Ray, Vector3, Vector3, Vector3);
bool test_ray_obb(Ray, OBB);
std::optional<Raycast_Hit> intersect_ray_obb(Ray, OBB);
bool test_ray_mesh(Ray, Mesh const&);
std::optional<Raycast_Hit> intersect_ray_mesh(Ray, Mesh const&, Matrix4 model_transform);
std::optional<Linecast_Hit> intersect_line_plane(Line, Vector3 plane_normal, float plane_distance);

#endif // !PHYSICS_INTERSECTION_TESTS_HPP_INCLUDE
