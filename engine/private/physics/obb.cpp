#include <physics/obb.hpp>

#include <anton/math/math.hpp>
#include <anton/math/transform.hpp>
#include <core/types.hpp>

namespace anton_engine {
  bool test_ray_obb(Ray ray, OBB obb)
  {
    Mat4 rotation = Mat4(Vec4{obb.local_x, 0}, Vec4{obb.local_y, 0},
                         Vec4{obb.local_z, 0}, Vec4{0, 0, 0, 1});
    // Center OBB at 0
    Mat4 obb_space = rotation * math::translate(-obb.center);
    Vec4 ray_dir = rotation * Vec4(ray.direction, 0);
    Vec4 ray_origin = obb_space * Vec4(ray.origin, 1);
    // AABB slab test
    f32 tmin = -math::infinity;
    f32 tmax = math::infinity;
    for(int i = 0; i < 3; ++i) {
      f32 tx1 = (obb.halfwidths[i] - ray_origin[i]) / ray_dir[i];
      f32 tx2 = (-obb.halfwidths[i] - ray_origin[i]) / ray_dir[i];
      tmax = math::min(tmax, math::max(tx1, tx2));
      tmin = math::max(tmin, math::min(tx1, tx2));
    }
    return tmax >= 0 && tmax >= tmin;
  }

  anton::Optional<Raycast_Hit> intersect_ray_obb(Ray ray, OBB obb)
  {
    Mat4 rotation = Mat4(Vec4{obb.local_x, 0}, Vec4{obb.local_y, 0},
                         Vec4{obb.local_z, 0}, Vec4{0, 0, 0, 1});
    // Center OBB at 0
    Mat4 obb_space = rotation * math::translate(-obb.center);
    Vec4 ray_dir = rotation * Vec4(ray.direction, 0);
    Vec4 ray_origin = obb_space * Vec4(ray.origin, 1);
    // AABB slab test
    f32 tmin = -math::infinity;
    f32 tmax = math::infinity;
    for(i32 i = 0; i < 3; ++i) {
      f32 tx1 = (obb.halfwidths[i] - ray_origin[i]) / ray_dir[i];
      f32 tx2 = (-obb.halfwidths[i] - ray_origin[i]) / ray_dir[i];
      tmax = math::min(tmax, math::max(tx1, tx2));
      tmin = math::max(tmin, math::min(tx1, tx2));
    }

    if(tmax >= 0 && tmax >= tmin) {
      Raycast_Hit out;
      out.distance = tmax;
      out.hit_point = ray.origin + ray.direction * tmax;
      return out;
    } else {
      return anton::null_optional;
    }
  }
} // namespace anton_engine
