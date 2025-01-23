#pragma once

#include <anton/math/mat4.hpp>
#include <anton/math/vec3.hpp>
#include <anton/optional.hpp>
#include <physics/intersections_common.hpp>
#include <physics/line.hpp>
#include <physics/obb.hpp>
#include <physics/ray.hpp>

namespace anton_engine {
  class Mesh;

  class Linecast_Hit {
  public:
    Vec3 hit_point;
    float distance = 0;
  };

  anton::Optional<Raycast_Hit> intersect_ray_triangle(Ray, Vec3, Vec3, Vec3);
  anton::Optional<Raycast_Hit> intersect_ray_quad(Ray, Vec3, Vec3, Vec3, Vec3);
  anton::Optional<Raycast_Hit> intersect_ray_plane(Ray, Vec3 plane_normal,
                                                   float plane_distance);
  anton::Optional<Raycast_Hit> intersect_ray_cone(Ray, Vec3 vertex,
                                                  Vec3 direction,
                                                  float angle_cos,
                                                  float height);
  anton::Optional<Raycast_Hit>
  intersect_ray_cylinder(Ray, Vec3 vertex1, Vec3 vertex2, float radius);
  bool test_ray_mesh(Ray, Mesh const&);
  anton::Optional<Raycast_Hit> intersect_ray_mesh(Ray, Mesh const&,
                                                  Mat4 model_transform);
  anton::Optional<Linecast_Hit> intersect_line_plane(Line, Vec3 plane_normal,
                                                     float plane_distance);
} // namespace anton_engine
