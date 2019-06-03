#include "intersection_tests.hpp"

#include "math/math.hpp"
#include "math/matrix3.hpp"
#include "math/matrix4.hpp"
#include "math/transform.hpp"
#include "mesh/mesh.hpp"

bool intersect_ray_triangle(Ray ray, Vector3 a, Vector3 b, Vector3 c, Raycast_Hit& out) {
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

    Vector3 r(a + u * ab + v * ac);
    // float distance = math::length(r - ray.origin);
    out = {r, {u, v, 1 - u - v}, t};
    return true;
}

bool test_ray_obb(Ray ray, OBB obb) {
    Matrix4 rotation = Matrix4(Vector4{obb.local_x, 0}, Vector4{obb.local_y, 0}, Vector4{obb.local_z, 0}, Vector4{0, 0, 0, 1});
    // Center OBB at 0
    Matrix4 obb_space = math::transform::translate(-obb.center) * rotation;
    Vector4 ray_dir = Vector4(ray.direction, 0) * rotation;
    Vector4 ray_origin = Vector4(ray.origin, 1) * obb_space;
    // AABB slab test
    float tmin = -math::constants<float>::infinity;
    float tmax = math::constants<float>::infinity;
    for (int i = 0; i < 3; ++i) {
        float tx1 = (obb.halfwidths[i] - ray_origin[i]) / ray_dir[i];
        float tx2 = (-obb.halfwidths[i] - ray_origin[i]) / ray_dir[i];
        tmax = math::min(tmax, math::max(tx1, tx2));
        tmin = math::max(tmin, math::min(tx1, tx2));
    }
    return tmax >= 0 && tmax >= tmin;
}

bool intersect_ray_obb(Ray ray, OBB obb, Raycast_Hit& out) {
    Matrix4 rotation = Matrix4(Vector4{obb.local_x, 0}, Vector4{obb.local_y, 0}, Vector4{obb.local_z, 0}, Vector4{0, 0, 0, 1});
    // Center OBB at 0
    Matrix4 obb_space = math::transform::translate(-obb.center) * rotation;
    Vector4 ray_dir = Vector4(ray.direction, 0) * rotation;
    Vector4 ray_origin = Vector4(ray.origin, 1) * obb_space;
    // AABB slab test
    float tmin = -math::constants<float>::infinity;
    float tmax = math::constants<float>::infinity;
    for (int i = 0; i < 3; ++i) {
        float tx1 = (obb.halfwidths[i] - ray_origin[i]) / ray_dir[i];
        float tx2 = (-obb.halfwidths[i] - ray_origin[i]) / ray_dir[i];
        tmax = math::min(tmax, math::max(tx1, tx2));
        tmin = math::max(tmin, math::min(tx1, tx2));
    }

    if (tmax >= 0 && tmax >= tmin) {
        out.distance = tmax;
        out.hit_point = ray.origin + ray.direction * tmax;
        return true;
    } else {
        return false;
    }
}

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