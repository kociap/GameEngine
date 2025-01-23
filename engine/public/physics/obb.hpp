#pragma once

#include <anton/math/vec3.hpp>
#include <anton/optional.hpp>
#include <core/types.hpp>
#include <physics/intersections_common.hpp>
#include <physics/ray.hpp>

namespace anton_engine {
  class OBB {
  public:
    Vec3 center;
    Vec3 local_x;
    Vec3 local_y;
    Vec3 local_z;
    Vec3 halfwidths;
  };

  [[nodiscard]] bool test_ray_obb(Ray, OBB);
  [[nodiscard]] anton::Optional<Raycast_Hit> intersect_ray_obb(Ray, OBB);
} // namespace anton_engine
