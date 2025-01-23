#include <level_editor/gizmo/gizmo.hpp>

namespace anton_engine::gizmo {
  float compute_scale(Mat4 const world_transform, u32 const target_size,
                      Mat4 const pers_mat, Vec2 const viewport_size)
  {
    float const pixel_size = 1 / viewport_size.y;
    float const projected_w_comp = (pers_mat * world_transform[3]).w;
    return /* scale_basis * */ target_size * pixel_size * projected_w_comp;
  }
} // namespace anton_engine::gizmo
