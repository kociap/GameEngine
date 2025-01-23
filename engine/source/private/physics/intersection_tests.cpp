#include <physics/intersection_tests.hpp>

#include <anton/math/mat3.hpp>
#include <anton/math/mat4.hpp>
#include <anton/math/math.hpp>
#include <anton/math/transform.hpp>
#include <core/types.hpp>
#include <engine/mesh.hpp>

namespace anton_engine {
  anton::Optional<Raycast_Hit> intersect_ray_plane(Ray const ray,
                                                   Vec3 const plane_normal,
                                                   f32 const plane_distance)
  {
    f32 const angle_cos = math::dot(ray.direction, plane_normal);
    f32 const coeff =
      (plane_distance - math::dot(ray.origin, plane_normal)) / angle_cos;
    if(math::abs(angle_cos) > math::epsilon && coeff >= 0.0f) {
      Raycast_Hit out;
      out.distance = coeff;
      out.hit_point = ray.origin + ray.direction * coeff;
      return out;
    } else {
      return anton::null_optional;
    }
  }

  anton::Optional<Raycast_Hit> intersect_ray_triangle(Ray const ray,
                                                      Vec3 const a,
                                                      Vec3 const b,
                                                      Vec3 const c)
  {
    Vec3 ao = ray.origin - a;
    Vec3 ab = b - a;
    Vec3 ac = c - a;
    Mat3 inv_mat = math::inverse({ab, ac, -ray.direction});
    Vec3 result = inv_mat * ao;
    f32 u = result.x;
    f32 v = result.y;
    f32 t = result.z;
    if(u < 0 || v < 0 || u + v > 1 || t < 0) {
      return anton::null_optional;
    }

    Vec3 r(a + u * ab + v * ac);
    return Raycast_Hit{r, {u, v, 1 - u - v}, t};
  }

  anton::Optional<Raycast_Hit>
  intersect_ray_cone(Ray const ray, Vec3 const vertex, Vec3 const direction,
                     f32 const angle_cos, f32 const height)
  {
    anton::Optional<Raycast_Hit> result = anton::null_optional;

    Vec3 const ray_origin = ray.origin - vertex;
    f32 const angle_cos_square = angle_cos * angle_cos;
    Vec3 const ray_dir_prim = math::dot(ray.direction, direction) * direction;
    f32 const a =
      angle_cos_square * math::length_squared(ray.direction - ray_dir_prim) -
      math::length_squared(ray_dir_prim);
    f32 const ray_origin_prim_len = math::dot(ray_origin, direction);
    Vec3 const ray_origin_prim = ray_origin_prim_len * direction;
    f32 const b =
      2.0f * angle_cos_square *
        math::dot(ray_origin - ray_origin_prim, ray.direction - ray_dir_prim) -
      2.0f * math::dot(ray_origin_prim, ray_dir_prim);
    f32 const c =
      angle_cos_square * math::length_squared(ray_origin - ray_origin_prim) -
      math::length_squared(ray_origin_prim);
    if(a > math::epsilon || a < -math::epsilon) {
      f32 const delta = b * b - 4.0f * a * c;
      if(delta >= 0.0f) {
        f32 const delta_sqrt = math::sqrt(delta);
        f32 const t1 = 0.5f * (-b - delta_sqrt) / a;
        Vec3 const t1v = ray_origin + ray.direction * t1;
        if(f32 const point_height = math::dot(direction, t1v);
           t1 >= 0.0f && point_height >= 0 && point_height <= height) {
          Raycast_Hit hit;
          hit.distance = t1;
          hit.hit_point = t1v + vertex;
          result = hit;
        }

        f32 const t2 = 0.5f * (-b + delta_sqrt) / a;
        Vec3 const t2v = ray_origin + ray.direction * t2;
        if(f32 const point_height = math::dot(direction, t2v);
           t2 >= 0.0f && point_height >= 0 && point_height <= height &&
           (!result || t2 < result->distance)) {
          Raycast_Hit hit;
          hit.distance = t2;
          hit.hit_point = t2v + vertex;
          result = hit;
        }
      }
    } else {
      // Ray is parallel to the cone boundary
      if(b > math::epsilon || b < math::epsilon) {
        f32 const t = -c / b;
        f32 const point_height =
          math::dot(ray_origin + ray.direction * t, direction);
        if(t >= 0 && point_height <= height && point_height >= 0.0f) {
          Raycast_Hit hit;
          hit.distance = t;
          hit.hit_point = ray.origin + ray.direction * t;
          result = hit;
        }
      } else {
        // Ray is on the boundary of the infinite cone
        if(ray_origin_prim_len <= height && ray_origin_prim_len >= 0.0f) {
          // Origin lies on the cone's coundary
          Raycast_Hit hit;
          hit.distance = 0.0f;
          hit.hit_point = ray.origin;
          result = hit;
        } else if(f32 const t = -math::dot(ray_origin, ray.direction);
                  t >= 0.0f) {
          Raycast_Hit hit;
          hit.distance = t;
          hit.hit_point = ray.origin + ray.direction * t;
          result = hit;
        }
      }
    }

    if(anton::Optional<Raycast_Hit> const hit =
         intersect_ray_plane({ray_origin, ray.direction}, direction, height);
       hit.holds_value() && (!result || hit->distance < result->distance) &&
       (math::length_squared(hit->hit_point -
                             math::dot(hit->hit_point, direction) * direction) *
          angle_cos_square <=
        height * height)) {
      result = hit;
    }

    return result;
  }

  anton::Optional<Raycast_Hit> intersect_ray_cylinder(Ray const ray,
                                                      Vec3 const vertex1,
                                                      Vec3 const vertex2,
                                                      f32 const radius)
  {
    anton::Optional<Raycast_Hit> result = anton::null_optional;

    f32 const radius_squared = radius * radius;
    Vec3 const ray_origin = ray.origin - vertex1;
    Vec3 const vert2 = vertex2 - vertex1;
    Vec3 const cylinder_normal = math::normalize(vert2);
    f32 const ray_dir_prim_len = math::dot(ray.direction, cylinder_normal);
    f32 const a =
      math::length_squared(ray.direction) - ray_dir_prim_len * ray_dir_prim_len;
    f32 const ray_origin_prim_len = math::dot(ray_origin, cylinder_normal);
    f32 const b = 2.0f * math::dot(ray_origin, ray.direction) -
                  2.0f * ray_origin_prim_len * ray_dir_prim_len;
    f32 const c = math::length_squared(ray_origin) -
                  ray_origin_prim_len * ray_origin_prim_len - radius_squared;
    f32 const cap2_plane_dist = math::dot(vert2, -cylinder_normal);
    if(a > math::epsilon || a < -math::epsilon) {
      f32 const delta = b * b - 4 * a * c;
      if(delta >= 0.0f) {
        f32 const delta_sqrt = math::sqrt(delta);
        f32 const t1 = 0.5f * (-b - delta_sqrt) / a;
        Vec3 const t1v = ray_origin + ray.direction * t1;
        if(t1 >= 0.0f && math::dot(t1v, cylinder_normal) >= 0 &&
           math::dot(t1v, -cylinder_normal) >= cap2_plane_dist) {
          Raycast_Hit hit;
          hit.distance = t1;
          hit.hit_point = t1v + vertex1;
          result = hit;
        }

        f32 const t2 = 0.5f * (-b + delta_sqrt) / a;
        Vec3 const t2v = ray_origin + ray.direction * t2;
        if(t2 >= 0.0f && (!result || t2 < t1) &&
           math::dot(t2v, cylinder_normal) >= 0 &&
           math::dot(t2v, -cylinder_normal) >= cap2_plane_dist) {
          Raycast_Hit hit;
          hit.distance = t2;
          hit.hit_point = t2v + vertex1;
          result = hit;
        }
      }
    } else {
      // Ray is parallel to the cylinder
      if(math::length_squared(ray_origin -
                              ray_origin_prim_len * cylinder_normal) ==
           radius_squared &&
         math::dot(ray_origin, cylinder_normal) >= 0 &&
         math::dot(ray_origin, -cylinder_normal) >= cap2_plane_dist) {
        // Ray origin lies on the cylinder boundary
        Raycast_Hit hit;
        hit.distance = 0.0f;
        hit.hit_point = ray.origin;
        result = hit;
      }
    }

    // Cap 1
    {
      f32 const angle_cos = math::dot(ray.direction, cylinder_normal);
      f32 const coeff = (-math::dot(ray.origin, cylinder_normal)) / angle_cos;
      Vec3 const hit_point = ray_origin + ray.direction * coeff;
      if(math::abs(angle_cos) > math::epsilon && coeff >= 0.0f &&
         (!result || coeff < result->distance) &&
         math::length_squared(hit_point) <= radius_squared) {
        Raycast_Hit hit;
        hit.distance = coeff;
        hit.hit_point = hit_point + vertex1;
        result = hit;
      }
    }

    // Cap 2
    {
      f32 const angle_cos = math::dot(ray.direction, -cylinder_normal);
      f32 const coeff =
        (cap2_plane_dist - math::dot(ray.origin, -cylinder_normal)) / angle_cos;
      Vec3 const hit_point = ray_origin + ray.direction * coeff;
      if(math::abs(angle_cos) > math::epsilon && coeff >= 0.0f &&
         (!result || coeff < result->distance) &&
         math::length_squared(hit_point - vert2) <= radius_squared) {
        Raycast_Hit hit;
        hit.distance = coeff;
        hit.hit_point = hit_point + vertex1;
        result = hit;
      }
    }

    return result;
  }

  bool test_ray_mesh(Ray ray, Mesh const& mesh)
  {
    auto& verts = mesh.vertices;
    for(isize i = 0; i < mesh.indices.size(); i += 3) {
      if(intersect_ray_triangle(ray, verts[mesh.indices[i]].position,
                                verts[mesh.indices[i + 1]].position,
                                verts[mesh.indices[i + 2]].position)) {
        return true;
      }
    }
    return false;
  }

  anton::Optional<Raycast_Hit> intersect_ray_mesh(Ray ray, Mesh const& mesh,
                                                  Mat4 model_transform)
  {
    bool hit_flag = false;
    Raycast_Hit closest_hit;
    closest_hit.distance = math::infinity;
    auto& verts = mesh.vertices;
    for(isize i = 0; i < mesh.indices.size(); i += 3) {
      Vec4 vert1 = model_transform * Vec4(verts[mesh.indices[i]].position, 1);
      Vec4 vert2 =
        model_transform * Vec4(verts[mesh.indices[i + 1]].position, 1);
      Vec4 vert3 =
        model_transform * Vec4(verts[mesh.indices[i + 2]].position, 1);
      if(auto hit =
           intersect_ray_triangle(ray, Vec3(vert1), Vec3(vert2), Vec3(vert3));
         hit && hit->distance < closest_hit.distance) {
        closest_hit = hit.value();
        hit_flag = true;
      }
    }

    if(hit_flag) {
      return closest_hit;
    } else {
      return anton::null_optional;
    }
  }

  anton::Optional<Linecast_Hit> intersect_line_plane(Line line, Vec3 normal,
                                                     f32 distance)
  {
    f32 angle_cos = math::dot(line.direction, normal);
    if(math::abs(angle_cos) > math::epsilon) {
      f32 coeff = (distance - math::dot(line.origin, normal)) / angle_cos;
      Linecast_Hit out;
      out.distance = coeff;
      out.hit_point = line.origin + line.direction * coeff;
      return out;
    } else {
      return anton::null_optional;
    }
  }
} // namespace anton_engine
