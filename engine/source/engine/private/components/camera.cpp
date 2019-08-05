#include "components/camera.hpp"
#include "components/transform.hpp"
#include "engine.hpp"
#include "math/math.hpp"
#include "math/transform.hpp"

Vector3 get_camera_top(Transform& transform) {
    // TODO return global instead of local probably
    Vector4 top = Vector4(0, 1, 0, 0) * math::transform::rotate(transform.local_rotation);
    return {top.x, top.y, top.z};
}

Vector3 get_camera_front(Transform& transform) {
    // TODO return global instead of local probably
    Vector4 front = Vector4(0, 0, -1, 0) * math::transform::rotate(transform.local_rotation);
    return {front.x, front.y, front.z};
}

Matrix4 get_camera_view_matrix(Transform& transform) {
    return math::transform::look_at(transform.local_position, transform.local_position + get_camera_front(transform), get_camera_top(transform));
}

Matrix4 get_camera_projection_matrix(Camera& camera, uint32_t width, uint32_t height) {
    float aspect_ratio = static_cast<float>(width) / static_cast<float>(height);
    return math::transform::perspective(math::radians(camera.fov), aspect_ratio, camera.near_plane, camera.far_plane);

    // Ortographic camera stuff
    // return math::transform::orthographic(-size * aspect_ratio, size * aspect_ratio, -size, size, near_plane, far_plane);
}

Vector3 screen_to_world_point(Matrix4 const inv_view, Matrix4 const inv_projection, uint32_t const screen_width, uint32_t const screen_height,
                              Vector2 const point) {
    // Transform screen point to normalized -1..1 coordinates
    float const normalized_x = 2.0f * point.x / static_cast<float>(screen_width) - 1.0f;
    float const normalized_y = 2.0f * point.y / static_cast<float>(screen_height) - 1.0f;

    Vector4 const ray_start = Vector4(normalized_x, normalized_y, -1.0f, 1.0f);
    Vector4 const ray_start_hg = ray_start * inv_projection;
    Vector3 ray_start_homogenized = Vector3(ray_start_hg);

    if (ray_start_hg.w != 0.0f) {
        ray_start_homogenized /= ray_start_hg.w;
    }

    Vector3 ray_start_world_space = Vector3(Vector4(ray_start_homogenized, 1.0f) * inv_view);
    return ray_start_world_space;
}