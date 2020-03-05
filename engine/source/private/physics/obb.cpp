#include <physics/obb.hpp>

#include <core/types.hpp>
#include <core/math/transform.hpp>

namespace anton_engine {
    bool test_ray_obb(Ray ray, OBB obb) {
        Matrix4 rotation = Matrix4(Vector4{obb.local_x, 0}, Vector4{obb.local_y, 0}, Vector4{obb.local_z, 0}, Vector4{0, 0, 0, 1});
        // Center OBB at 0
        Matrix4 obb_space = math::transform::translate(-obb.center) * rotation;
        Vector4 ray_dir = Vector4(ray.direction, 0) * rotation;
        Vector4 ray_origin = Vector4(ray.origin, 1) * obb_space;
        // AABB slab test
        float tmin = -math::constants::infinity;
        float tmax = math::constants::infinity;
        for (int i = 0; i < 3; ++i) {
            float tx1 = (obb.halfwidths[i] - ray_origin[i]) / ray_dir[i];
            float tx2 = (-obb.halfwidths[i] - ray_origin[i]) / ray_dir[i];
            tmax = math::min(tmax, math::max(tx1, tx2));
            tmin = math::max(tmin, math::min(tx1, tx2));
        }
        return tmax >= 0 && tmax >= tmin;
    }

    anton_stl::Optional<Raycast_Hit> intersect_ray_obb(Ray ray, OBB obb) {
        Matrix4 rotation = Matrix4(Vector4{obb.local_x, 0}, Vector4{obb.local_y, 0}, Vector4{obb.local_z, 0}, Vector4{0, 0, 0, 1});
        // Center OBB at 0
        Matrix4 obb_space = math::transform::translate(-obb.center) * rotation;
        Vector4 ray_dir = Vector4(ray.direction, 0) * rotation;
        Vector4 ray_origin = Vector4(ray.origin, 1) * obb_space;
        // AABB slab test
        float tmin = -math::constants::infinity;
        float tmax = math::constants::infinity;
        for (int i = 0; i < 3; ++i) {
            float tx1 = (obb.halfwidths[i] - ray_origin[i]) / ray_dir[i];
            float tx2 = (-obb.halfwidths[i] - ray_origin[i]) / ray_dir[i];
            tmax = math::min(tmax, math::max(tx1, tx2));
            tmin = math::max(tmin, math::min(tx1, tx2));
        }

        if (tmax >= 0 && tmax >= tmin) {
            Raycast_Hit out;
            out.distance = tmax;
            out.hit_point = ray.origin + ray.direction * tmax;
            return out;
        } else {
            return anton_stl::null_optional;
        }
    }
} // namespace anton_engine
