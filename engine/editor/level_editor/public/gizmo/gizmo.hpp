#ifndef EDITOR_LEVEL_EDITOR_GIZMO_GIZMO_HPP_INCLUDE
#define EDITOR_LEVEL_EDITOR_GIZMO_GIZMO_HPP_INCLUDE

#include <cstdint>
#include <math/matrix4.hpp>
#include <math/vector2.hpp>

namespace anton_engine::gizmo {
    [[nodiscard]] float compute_scale(Matrix4 world_transform, uint32_t target_size, Matrix4 pers_mat, Vector2 viewport_size);
} // namespace anton_engine::gizmo

#endif // !EDITOR_LEVEL_EDITOR_GIZMO_GIZMO_HPP_INCLUDE
