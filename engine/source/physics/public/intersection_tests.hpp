#ifndef PHYSICS_INTERSECTION_TESTS_HPP_INCLUDE
#define PHYSICS_INTERSECTION_TESTS_HPP_INCLUDE

#include <aabb.hpp>
#include <anton_stl/optional.hpp>
#include <intersections_common.hpp>
#include <line.hpp>
#include <math/matrix4.hpp>
#include <math/vector3.hpp>
#include <obb.hpp>
#include <ray.hpp>

namespace anton_engine {
    class Mesh;

    class Linecast_Hit {
    public:
        Vector3 hit_point;
        float distance = 0;
    };

    anton_stl::Optional<Raycast_Hit> intersect_ray_triangle(Ray, Vector3, Vector3, Vector3);
    anton_stl::Optional<Raycast_Hit> intersect_ray_quad(Ray, Vector3, Vector3, Vector3, Vector3);
    bool test_ray_mesh(Ray, Mesh const&);
    anton_stl::Optional<Raycast_Hit> intersect_ray_mesh(Ray, Mesh const&, Matrix4 model_transform);
    anton_stl::Optional<Linecast_Hit> intersect_line_plane(Line, Vector3 plane_normal, float plane_distance);
} // namespace anton_engine

#endif // !PHYSICS_INTERSECTION_TESTS_HPP_INCLUDE
