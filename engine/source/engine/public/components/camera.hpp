#ifndef ENGINE_COMPONENTS_CAMERA_HPP_INCLUDE
#define ENGINE_COMPONENTS_CAMERA_HPP_INCLUDE

#include <class_macros.hpp>
#include <components/transform.hpp>
#include <math/matrix4.hpp>
#include <math/transform.hpp>
#include <math/vector2.hpp>
#include <math/vector3.hpp>
#include <serialization/serialization.hpp>

#include <cstdint>

namespace anton_engine {
    class COMPONENT Camera {
    public:
        // Size of the camera in ortographic projection
        // float size = 5;
        // Field of view in perspective projection
        float fov = 70;
        float near_plane = 0.3f;
        float far_plane = 1000.0f;
        bool active = true; // Temporarily make active by default
    };

    inline Vector3 get_camera_top(Transform& transform) {
        // TODO: Return global instead of local
        Vector4 top = Vector4(0, 1, 0, 0) * math::transform::rotate(transform.local_rotation);
        return {top.x, top.y, top.z};
    }

    inline Vector3 get_camera_front(Transform& transform) {
        // TODO: Return global instead of local
        Vector4 front = Vector4(0, 0, -1, 0) * math::transform::rotate(transform.local_rotation);
        return {front.x, front.y, front.z};
    }

    inline Matrix4 get_camera_view_matrix(Transform& transform) {
        return math::transform::look_at(transform.local_position, transform.local_position + get_camera_front(transform), get_camera_top(transform));
    }

    inline Matrix4 get_camera_projection_matrix(Camera& camera, int32_t viewport_width, int32_t viewport_height) {
        float aspect_ratio = static_cast<float>(viewport_width) / static_cast<float>(viewport_height);
        return math::transform::perspective(math::radians(camera.fov), aspect_ratio, camera.near_plane, camera.far_plane);

        // Ortographic camera stuff
        // return math::transform::orthographic(-size * aspect_ratio, size * aspect_ratio, -size, size, near_plane, far_plane);
    }

    // inline Vector3 screen_to_world_point(Matrix4 const inv_view, Matrix4 const inv_projection, int32_t const screen_width, int32_t const screen_height,
    //                               Vector2 const point) {
    //     // Transform screen point to normalized -1..1 coordinates
    //     float const normalized_x = 2.0f * point.x / static_cast<float>(screen_width) - 1.0f;
    //     float const normalized_y = 2.0f * point.y / static_cast<float>(screen_height) - 1.0f;

    //     Vector4 const ray_start = Vector4(normalized_x, normalized_y, -1.0f, 1.0f);
    //     Vector4 const ray_start_hg = ray_start * inv_projection;
    //     Vector3 ray_start_homogenized = Vector3(ray_start_hg);

    //     if (ray_start_hg.w != 0.0f) {
    //         ray_start_homogenized /= ray_start_hg.w;
    //     }

    //     Vector3 ray_start_world_space = Vector3(Vector4(ray_start_homogenized, 1.0f) * inv_view);
    //     return ray_start_world_space;
    // }
} // namespace anton_engine

ANTON_DEFAULT_SERIALIZABLE(anton_engine::Camera);

#endif // !ENGINE_COMPONENTS_CAMERA_HPP_INCLUDE
