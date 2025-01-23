#include <physics/ray.hpp>

namespace anton_engine {
  Ray screen_to_ray(Mat4 inv_view, Mat4 inv_projection, i32 screen_width,
                    i32 screen_height, Vec2 point)
  {
    // Transform screen point to normalized -1..1 coordinates
    float const normalized_x =
      2.0f * point.x / static_cast<float>(screen_width) - 1.0f;
    float const normalized_y =
      2.0f * point.y / static_cast<float>(screen_height) - 1.0f;

    Vec4 const ray_start = Vec4(normalized_x, normalized_y, -1.0f, 1.0f);
    Vec4 const ray_end = Vec4(normalized_x, normalized_y, 0.0f, 1.0f);

    Vec4 const ray_start_hg = inv_projection * ray_start;
    Vec4 const ray_end_hg = inv_projection * ray_end;

    Vec3 ray_start_homogenized = Vec3(ray_start_hg);
    Vec3 ray_end_homogenized = Vec3(ray_end_hg);

    if(ray_start_hg.w != 0.0f) {
      ray_start_homogenized /= ray_start_hg.w;
    }
    if(ray_end_hg.w != 0.0f) {
      ray_end_homogenized /= ray_end_hg.w;
    }

    Vec3 ray_direction_view_space = ray_end_homogenized - ray_start_homogenized;
    ray_direction_view_space = math::normalize(ray_direction_view_space);

    Vec3 ray_start_world_space =
      Vec3(inv_view * Vec4(ray_start_homogenized, 1.0f));
    Vec3 ray_direction_world_space =
      Vec3(inv_view * Vec4(ray_direction_view_space, 0.0f));
    ray_direction_world_space = math::normalize(ray_direction_world_space);

    return {ray_start_world_space, ray_direction_world_space};
  }

  Ray screen_to_ray(Mat4 inv_view_projection_matrix, i32 screen_width,
                    i32 screen_height, Vec2 point)
  {
    // Transform screen point to normalized -1..1 coordinates
    float const normalized_x =
      2.0f * point.x / static_cast<float>(screen_width) - 1.0f;
    float const normalized_y =
      2.0f * point.y / static_cast<float>(screen_height) - 1.0f;

    // Since z=1 is far and z=-1 is near in NDC, we use -1 for the start and 0 for the end to get better precision.
    Vec4 const ray_start = Vec4(normalized_x, normalized_y, -1.0f, 1.0f);
    Vec4 const ray_end = Vec4(normalized_x, normalized_y, 0.0f, 1.0f);

    Vec4 const ray_start_world_space = inv_view_projection_matrix * ray_start;
    Vec4 const ray_end_world_space = inv_view_projection_matrix * ray_end;

    Vec3 ray_start_homogenized = Vec3(ray_start_world_space);
    Vec3 ray_end_homogenized = Vec3(ray_end_world_space);

    if(ray_start_world_space.w != 0.0f) {
      ray_start_homogenized /= ray_start_world_space.w;
    }
    if(ray_end_world_space.w != 0.0f) {
      ray_end_homogenized /= ray_end_world_space.w;
    }

    Vec3 ray_direction = ray_end_homogenized - ray_start_homogenized;
    ray_direction = math::normalize(ray_direction);

    return {ray_start_homogenized, ray_direction};
  }
} // namespace anton_engine
