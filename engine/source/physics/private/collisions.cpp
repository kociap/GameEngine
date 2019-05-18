#include "collisions.hpp"

#include "math/math.hpp"
#include "mesh/mesh.hpp"

namespace physics {
    bool intersect_ray_triangle(Ray ray, Point a, Point b, Point c, Raycast_Hit& out) {
        Vector3 ao = ray.origin - a;
        Vector3 ab = b - a;
        Vector3 ac = c - a;

        Vector3 n = math::cross(ab, ac);
        float d = -math::dot(ray.direction, n);
        // TODO if d == 0, ray is parallel to the plane of the triangle
        float t = math::dot(ao, n / d);
        if (t < 0.0f) {
            return false;
        }

        Vector3 e = math::cross(ray.direction, ao);
        float u = -math::dot(e, ac);
        if (u < 0.0f || u > d) {
            return false;
        }
        float v = -math::dot(e, ab);
        if (v < 0.0f || u + v > d) {
            return false;
        }
        t /= d;
        u /= d;
        v /= d;

        Point r(a + b * u + c * v);
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