#pragma once

#include <anton/math/mat4.hpp>
#include <anton/math/transform.hpp>
#include <anton/math/vec2.hpp>
#include <anton/math/vec3.hpp>
#include <core/class_macros.hpp>
#include <core/serialization/serialization.hpp>
#include <core/types.hpp>
#include <engine/components/transform.hpp>

namespace anton_engine {
  class COMPONENT Camera {
  public:
    // Size of the camera in ortographic projection
    // float size = 5;
    // Field of view in perspective projection
    f32 fov = 70;
    f32 near_plane = 0.3f;
    f32 far_plane = 1000.0f;
    bool active = true; // Temporarily make active by default
  };

  inline Vec3 get_camera_top(Transform& transform)
  {
    // TODO: Return global instead of local
    Vec4 top = math::rotate(transform.local_rotation) * Vec4(0, 1, 0, 0);
    return {top.x, top.y, top.z};
  }

  inline Vec3 get_camera_front(Transform& transform)
  {
    // TODO: Return global instead of local
    Vec4 front = math::rotate(transform.local_rotation) * Vec4(0, 0, -1, 0);
    return {front.x, front.y, front.z};
  }

  inline Mat4 get_camera_view_matrix(Transform& transform)
  {
    Mat4 const camera_matrix = to_matrix(transform);
    return math::inverse(camera_matrix);
  }

  inline Mat4 get_camera_projection_matrix(Camera& camera, i32 viewport_width,
                                           i32 viewport_height)
  {
    float aspect_ratio =
      static_cast<float>(viewport_width) / static_cast<float>(viewport_height);
    return math::perspective_rh(math::radians(camera.fov), aspect_ratio,
                                camera.near_plane, camera.far_plane);

    // Ortographic camera stuff
    // return math::orthographic(-size * aspect_ratio, size * aspect_ratio, -size, size, near_plane, far_plane);
  }

  inline Mat4 get_camera_projection_matrix(Camera const camera,
                                           Vec2 const viewport_size)
  {
    f32 const aspect_ratio = viewport_size.x / viewport_size.y;
    return math::perspective_rh(math::radians(camera.fov), aspect_ratio,
                                camera.near_plane, camera.far_plane);

    // Ortographic camera stuff
    // return math::orthographic(-size * aspect_ratio, size * aspect_ratio, -size, size, near_plane, far_plane);
  }

  // inline Vec3 screen_to_world_point(Mat4 const inv_view, Mat4 const inv_projection, i32 const screen_width, i32 const screen_height,
  //                               Vec2 const point) {
  //     // Transform screen point to normalized -1..1 coordinates
  //     float const normalized_x = 2.0f * point.x / static_cast<float>(screen_width) - 1.0f;
  //     float const normalized_y = 2.0f * point.y / static_cast<float>(screen_height) - 1.0f;

  //     Vec4 const ray_start = Vec4(normalized_x, normalized_y, -1.0f, 1.0f);
  //     Vec4 const ray_start_hg = ray_start * inv_projection;
  //     Vec3 ray_start_homogenized = Vec3(ray_start_hg);

  //     if (ray_start_hg.w != 0.0f) {
  //         ray_start_homogenized /= ray_start_hg.w;
  //     }

  //     Vec3 ray_start_world_space = Vec3(Vec4(ray_start_homogenized, 1.0f) * inv_view);
  //     return ray_start_world_space;
  // }
} // namespace anton_engine

ANTON_DEFAULT_SERIALIZABLE(anton_engine::Camera)
