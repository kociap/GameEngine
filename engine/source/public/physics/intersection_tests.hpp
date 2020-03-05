#ifndef PHYSICS_INTERSECTION_TESTS_HPP_INCLUDE
#define PHYSICS_INTERSECTION_TESTS_HPP_INCLUDE

#include <core/stl/optional.hpp>
#include <physics/intersections_common.hpp>
#include <physics/line.hpp>
#include <core/math/matrix4.hpp>
#include <core/math/vector3.hpp>
#include <physics/obb.hpp>
#include <physics/ray.hpp>

namespace anton_engine {
    class Mesh;

    class Linecast_Hit {
    public:
        Vector3 hit_point;
        float distance = 0;
    };

    anton_stl::Optional<Raycast_Hit> intersect_ray_triangle(Ray, Vector3, Vector3, Vector3);
    anton_stl::Optional<Raycast_Hit> intersect_ray_quad(Ray, Vector3, Vector3, Vector3, Vector3);
    anton_stl::Optional<Raycast_Hit> intersect_ray_plane(Ray, Vector3 plane_normal, float plane_distance);
    anton_stl::Optional<Raycast_Hit> intersect_ray_cone(Ray, Vector3 vertex, Vector3 direction, float angle_cos, float height);
    anton_stl::Optional<Raycast_Hit> intersect_ray_cylinder(Ray, Vector3 vertex1, Vector3 vertex2, float radius);
    bool test_ray_mesh(Ray, Mesh const&);
    anton_stl::Optional<Raycast_Hit> intersect_ray_mesh(Ray, Mesh const&, Matrix4 model_transform);
    anton_stl::Optional<Linecast_Hit> intersect_line_plane(Line, Vector3 plane_normal, float plane_distance);
} // namespace anton_engine

#endif // !PHYSICS_INTERSECTION_TESTS_HPP_INCLUDE
