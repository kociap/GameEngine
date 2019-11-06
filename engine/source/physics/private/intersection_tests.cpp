#include <intersection_tests.hpp>

#include <math/math.hpp>
#include <math/matrix3.hpp>
#include <math/matrix4.hpp>
#include <math/transform.hpp>
#include <mesh.hpp>

namespace anton_engine {
    anton_stl::Optional<Raycast_Hit> intersect_ray_triangle(Ray ray, Vector3 a, Vector3 b, Vector3 c) {
        Vector3 ao = ray.origin - a;
        Vector3 ab = b - a;
        Vector3 ac = c - a;
        Matrix3 inv_mat = math::inverse({ab, ac, -ray.direction});
        Vector3 result = ao * inv_mat;
        float u = result.x;
        float v = result.y;
        float t = result.z;
        if (u < 0 || v < 0 || u + v > 1 || t < 0) {
            return anton_stl::null_optional;
        }

        Vector3 r(a + u * ab + v * ac);
        return Raycast_Hit{r, {u, v, 1 - u - v}, t};
    }

    bool test_ray_mesh(Ray ray, Mesh const& mesh) {
        auto& verts = mesh.vertices;
        // TODO size_t if I ever define it
        for (int64_t i = 0; i < mesh.indices.size(); i += 3) {
            if (intersect_ray_triangle(ray, verts[mesh.indices[i]].position, verts[mesh.indices[i + 1]].position, verts[mesh.indices[i + 2]].position)) {
                return true;
            }
        }
        return false;
    }

    anton_stl::Optional<Raycast_Hit> intersect_ray_mesh(Ray ray, Mesh const& mesh, Matrix4 model_transform) {
        bool hit_flag = false;
        Raycast_Hit closest_hit;
        closest_hit.distance = math::constants::infinity;
        auto& verts = mesh.vertices;
        // TODO size_t if I ever define it
        for (int64_t i = 0; i < mesh.indices.size(); i += 3) {
            Vector4 vert1 = Vector4(verts[mesh.indices[i]].position, 1) * model_transform;
            Vector4 vert2 = Vector4(verts[mesh.indices[i + 1]].position, 1) * model_transform;
            Vector4 vert3 = Vector4(verts[mesh.indices[i + 2]].position, 1) * model_transform;
            if (auto hit = intersect_ray_triangle(ray, Vector3(vert1), Vector3(vert2), Vector3(vert3)); hit && hit->distance < closest_hit.distance) {
                closest_hit = hit.value();
                hit_flag = true;
            }
        }

        if (hit_flag) {
            return closest_hit;
        } else {
            return anton_stl::null_optional;
        }
    }

    anton_stl::Optional<Linecast_Hit> intersect_line_plane(Line line, Vector3 normal, float distance) {
        float angle_cos = math::dot(line.direction, normal);
        if (math::abs(angle_cos) > math::constants::epsilon) {
            float coeff = (distance - math::dot(line.origin, normal)) / angle_cos;
            Linecast_Hit out;
            out.distance = coeff;
            out.hit_point = line.origin + line.direction * coeff;
            return out;
        } else {
            return anton_stl::null_optional;
        }
    }
} // namespace anton_engine
