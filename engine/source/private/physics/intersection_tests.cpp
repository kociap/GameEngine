#include <physics/intersection_tests.hpp>

#include <core/math/math.hpp>
#include <core/math/matrix3.hpp>
#include <core/math/matrix4.hpp>
#include <core/math/transform.hpp>
#include <core/types.hpp>
#include <engine/mesh.hpp>

namespace anton_engine {
    atl::Optional<Raycast_Hit> intersect_ray_plane(Ray const ray, Vector3 const plane_normal, float const plane_distance) {
        float const angle_cos = math::dot(ray.direction, plane_normal);
        float const coeff = (plane_distance - math::dot(ray.origin, plane_normal)) / angle_cos;
        if(math::abs(angle_cos) > math::epsilon && coeff >= 0.0f) {
            Raycast_Hit out;
            out.distance = coeff;
            out.hit_point = ray.origin + ray.direction * coeff;
            return out;
        } else {
            return atl::null_optional;
        }
    }

    atl::Optional<Raycast_Hit> intersect_ray_triangle(Ray const ray, Vector3 const a, Vector3 const b, Vector3 const c) {
        Vector3 ao = ray.origin - a;
        Vector3 ab = b - a;
        Vector3 ac = c - a;
        Matrix3 inv_mat = math::inverse({ab, ac, -ray.direction});
        Vector3 result = ao * inv_mat;
        float u = result.x;
        float v = result.y;
        float t = result.z;
        if(u < 0 || v < 0 || u + v > 1 || t < 0) {
            return atl::null_optional;
        }

        Vector3 r(a + u * ab + v * ac);
        return Raycast_Hit{r, {u, v, 1 - u - v}, t};
    }

    atl::Optional<Raycast_Hit> intersect_ray_cone(Ray const ray, Vector3 const vertex, Vector3 const direction, float const angle_cos, float const height) {
        atl::Optional<Raycast_Hit> result = atl::null_optional;

        Vector3 const ray_origin = ray.origin - vertex;
        float const angle_cos_square = angle_cos * angle_cos;
        Vector3 const ray_dir_prim = math::dot(ray.direction, direction) * direction;
        float const a = angle_cos_square * math::length_squared(ray.direction - ray_dir_prim) - math::length_squared(ray_dir_prim);
        float const ray_origin_prim_len = math::dot(ray_origin, direction);
        Vector3 const ray_origin_prim = ray_origin_prim_len * direction;
        float const b =
            2.0f * angle_cos_square * math::dot(ray_origin - ray_origin_prim, ray.direction - ray_dir_prim) - 2.0f * math::dot(ray_origin_prim, ray_dir_prim);
        float const c = angle_cos_square * math::length_squared(ray_origin - ray_origin_prim) - math::length_squared(ray_origin_prim);
        if(a > math::epsilon || a < -math::epsilon) {
            float const delta = b * b - 4.0f * a * c;
            if(delta >= 0.0f) {
                float const delta_sqrt = math::sqrt(delta);
                float const t1 = 0.5f * (-b - delta_sqrt) / a;
                Vector3 const t1v = ray_origin + ray.direction * t1;
                if(float const point_height = math::dot(direction, t1v); t1 >= 0.0f && point_height >= 0 && point_height <= height) {
                    Raycast_Hit hit;
                    hit.distance = t1;
                    hit.hit_point = t1v + vertex;
                    result = hit;
                }

                float const t2 = 0.5f * (-b + delta_sqrt) / a;
                Vector3 const t2v = ray_origin + ray.direction * t2;
                if(float const point_height = math::dot(direction, t2v);
                   t2 >= 0.0f && point_height >= 0 && point_height <= height && (!result || t2 < result->distance)) {
                    Raycast_Hit hit;
                    hit.distance = t2;
                    hit.hit_point = t2v + vertex;
                    result = hit;
                }
            }
        } else {
            // Ray is parallel to the cone boundary
            if(b > math::epsilon || b < math::epsilon) {
                float const t = -c / b;
                float const point_height = math::dot(ray_origin + ray.direction * t, direction);
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
                } else if(float const t = -math::dot(ray_origin, ray.direction); t >= 0.0f) {
                    Raycast_Hit hit;
                    hit.distance = t;
                    hit.hit_point = ray.origin + ray.direction * t;
                    result = hit;
                }
            }
        }

        if(atl::Optional<Raycast_Hit> const hit = intersect_ray_plane({ray_origin, ray.direction}, direction, height);
           hit.holds_value() && (!result || hit->distance < result->distance) &&
           (math::length_squared(hit->hit_point - math::dot(hit->hit_point, direction) * direction) * angle_cos_square <= height * height)) {
            result = hit;
        }

        return result;
    }

    atl::Optional<Raycast_Hit> intersect_ray_cylinder(Ray const ray, Vector3 const vertex1, Vector3 const vertex2, float const radius) {
        atl::Optional<Raycast_Hit> result = atl::null_optional;

        float const radius_squared = radius * radius;
        Vector3 const ray_origin = ray.origin - vertex1;
        Vector3 const vert2 = vertex2 - vertex1;
        Vector3 const cylinder_normal = math::normalize(vert2);
        float const ray_dir_prim_len = math::dot(ray.direction, cylinder_normal);
        float const a = math::length_squared(ray.direction) - ray_dir_prim_len * ray_dir_prim_len;
        float const ray_origin_prim_len = math::dot(ray_origin, cylinder_normal);
        float const b = 2.0f * math::dot(ray_origin, ray.direction) - 2.0f * ray_origin_prim_len * ray_dir_prim_len;
        float const c = math::length_squared(ray_origin) - ray_origin_prim_len * ray_origin_prim_len - radius_squared;
        float const cap2_plane_dist = math::dot(vert2, -cylinder_normal);
        if(a > math::epsilon || a < -math::epsilon) {
            float const delta = b * b - 4 * a * c;
            if(delta >= 0.0f) {
                float const delta_sqrt = math::sqrt(delta);
                float const t1 = 0.5f * (-b - delta_sqrt) / a;
                Vector3 const t1v = ray_origin + ray.direction * t1;
                if(t1 >= 0.0f && math::dot(t1v, cylinder_normal) >= 0 && math::dot(t1v, -cylinder_normal) >= cap2_plane_dist) {
                    Raycast_Hit hit;
                    hit.distance = t1;
                    hit.hit_point = t1v + vertex1;
                    result = hit;
                }

                float const t2 = 0.5f * (-b + delta_sqrt) / a;
                Vector3 const t2v = ray_origin + ray.direction * t2;
                if(t2 >= 0.0f && (!result || t2 < t1) && math::dot(t2v, cylinder_normal) >= 0 && math::dot(t2v, -cylinder_normal) >= cap2_plane_dist) {
                    Raycast_Hit hit;
                    hit.distance = t2;
                    hit.hit_point = t2v + vertex1;
                    result = hit;
                }
            }
        } else {
            // Ray is parallel to the cylinder
            if(math::length_squared(ray_origin - ray_origin_prim_len * cylinder_normal) == radius_squared && math::dot(ray_origin, cylinder_normal) >= 0 &&
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
            float const angle_cos = math::dot(ray.direction, cylinder_normal);
            float const coeff = (-math::dot(ray.origin, cylinder_normal)) / angle_cos;
            Vector3 const hit_point = ray_origin + ray.direction * coeff;
            if(math::abs(angle_cos) > math::epsilon && coeff >= 0.0f && (!result || coeff < result->distance) &&
               math::length_squared(hit_point) <= radius_squared) {
                Raycast_Hit hit;
                hit.distance = coeff;
                hit.hit_point = hit_point + vertex1;
                result = hit;
            }
        }

        // Cap 2
        {
            float const angle_cos = math::dot(ray.direction, -cylinder_normal);
            float const coeff = (cap2_plane_dist - math::dot(ray.origin, -cylinder_normal)) / angle_cos;
            Vector3 const hit_point = ray_origin + ray.direction * coeff;
            if(math::abs(angle_cos) > math::epsilon && coeff >= 0.0f && (!result || coeff < result->distance) &&
               math::length_squared(hit_point - vert2) <= radius_squared) {
                Raycast_Hit hit;
                hit.distance = coeff;
                hit.hit_point = hit_point + vertex1;
                result = hit;
            }
        }

        return result;
    }

    bool test_ray_mesh(Ray ray, Mesh const& mesh) {
        auto& verts = mesh.vertices;
        for(isize i = 0; i < mesh.indices.size(); i += 3) {
            if(intersect_ray_triangle(ray, verts[mesh.indices[i]].position, verts[mesh.indices[i + 1]].position, verts[mesh.indices[i + 2]].position)) {
                return true;
            }
        }
        return false;
    }

    atl::Optional<Raycast_Hit> intersect_ray_mesh(Ray ray, Mesh const& mesh, Matrix4 model_transform) {
        bool hit_flag = false;
        Raycast_Hit closest_hit;
        closest_hit.distance = math::infinity;
        auto& verts = mesh.vertices;
        for(isize i = 0; i < mesh.indices.size(); i += 3) {
            Vector4 vert1 = Vector4(verts[mesh.indices[i]].position, 1) * model_transform;
            Vector4 vert2 = Vector4(verts[mesh.indices[i + 1]].position, 1) * model_transform;
            Vector4 vert3 = Vector4(verts[mesh.indices[i + 2]].position, 1) * model_transform;
            if(auto hit = intersect_ray_triangle(ray, Vector3(vert1), Vector3(vert2), Vector3(vert3)); hit && hit->distance < closest_hit.distance) {
                closest_hit = hit.value();
                hit_flag = true;
            }
        }

        if(hit_flag) {
            return closest_hit;
        } else {
            return atl::null_optional;
        }
    }

    atl::Optional<Linecast_Hit> intersect_line_plane(Line line, Vector3 normal, float distance) {
        float angle_cos = math::dot(line.direction, normal);
        if(math::abs(angle_cos) > math::epsilon) {
            float coeff = (distance - math::dot(line.origin, normal)) / angle_cos;
            Linecast_Hit out;
            out.distance = coeff;
            out.hit_point = line.origin + line.direction * coeff;
            return out;
        } else {
            return atl::null_optional;
        }
    }
} // namespace anton_engine
