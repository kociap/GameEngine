#include "collisions.hpp"

#include "math/math.hpp"
#include "math/matrix3.hpp"
#include "mesh/mesh.hpp"

namespace physics {
    bool intersect_ray_triangle(Ray ray, Point a, Point b, Point c, Raycast_Hit& out) {
        Vector3 ao = ray.origin - a;
        Vector3 ab = b - a;
        Vector3 ac = c - a;
        Matrix3 inv_mat = math::inverse({ab, ac, -ray.direction});
        Vector3 result = ao * inv_mat;
        float u = result.x;
        float v = result.y;
        float t = result.z;
        if (u < 0 || v < 0 || u + v > 1 || t < 0) {
            return false;
        }

        Point r(a + u * ab + v * ac);
        // float distance = math::length(r - ray.origin);
        out = {r, {u, v, 1 - u - v}, t};
        return true;
    }

    // Raycast_Hit intersect_ray_triangle(Ray ray, Point a, Point b, Point c, Vector3 triangle_normal) {
    //     float dot = math::dot(ray.direction, triangle_normal);
    //     if (dot <= 0) {
    //         // not backfacing
    //         float t = 0;
    //         if (t > 0) {
    //         } else {
    //             return {Point(), false};
    //         }
    //     } else {
    //         return {Point(), false};
    //     }
    // }

    bool test_ray_mesh(Ray ray, Mesh const& mesh) {
        auto& verts = mesh.vertices;
        // TODO size_t if I ever define it
        for (uint64_t i = 0; i < mesh.indices.size(); i += 3) {
            Raycast_Hit hit;
            if (intersect_ray_triangle(ray, verts[mesh.indices[i]].position, verts[mesh.indices[i + 1]].position, verts[mesh.indices[i + 2]].position, hit)) {
                return true;
            }
        }
        return false;
    }

    bool intersect_ray_mesh(Ray ray, Mesh const& mesh, Matrix4 model_transform, Raycast_Hit& out) {
        bool hit_flag = false;
        Raycast_Hit closest_hit;
        closest_hit.distance = math::constants<float>::infinity;
        auto& verts = mesh.vertices;
        // TODO size_t if I ever define it
        for (uint64_t i = 0; i < mesh.indices.size(); i += 3) {
            Raycast_Hit hit;
            Vector4 vert1 = Vector4(verts[mesh.indices[i]].position, 1) * model_transform;
            Vector4 vert2 = Vector4(verts[mesh.indices[i + 1]].position, 1) * model_transform;
            Vector4 vert3 = Vector4(verts[mesh.indices[i + 2]].position, 1) * model_transform;
            if (intersect_ray_triangle(ray, Vector3(vert1), Vector3(vert2), Vector3(vert3), hit)) {
                if (hit.distance < closest_hit.distance) {
                    closest_hit = hit;
                    out = hit;
                    hit_flag = true;
                }
            }
        }
        return hit_flag;
    }
} // namespace physics