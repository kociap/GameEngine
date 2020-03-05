#ifndef PHYSICS_OBB_HPP_INCLUDE
#define PHYSICS_OBB_HPP_INCLUDE

#include <core/stl/optional.hpp>
#include <physics/intersections_common.hpp>
#include <core/math/vector3.hpp>
#include <physics/ray.hpp>

namespace anton_engine {
    class OBB {
    public:
        Vector3 center;
        Vector3 local_x;
        Vector3 local_y;
        Vector3 local_z;
        Vector3 halfwidths;
    };

    [[nodiscard]] bool test_ray_obb(Ray, OBB);
    [[nodiscard]] anton_stl::Optional<Raycast_Hit> intersect_ray_obb(Ray, OBB);
} // namespace anton_engine

#endif // !PHYSICS_OBB_HPP_INCLUDE
