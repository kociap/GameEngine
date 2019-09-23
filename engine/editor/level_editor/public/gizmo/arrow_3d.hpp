#ifndef EDITOR_LEVEL_EDITOR_GIZMO_ARROW_3D_HPP_INCLUDE
#define EDITOR_LEVEL_EDITOR_GIZMO_ARROW_3D_HPP_INCLUDE

#include <color.hpp>
#include <gizmo/gizmo.hpp>
#include <math/matrix4.hpp>
#include <math/vector2.hpp>

namespace anton_engine::gizmo {
    // Arrow head style
    enum class Arrow_3D_Style {
        cone,
        cube,
    };

    struct Arrow_3D {
        Arrow_3D_Style draw_style;
        Color color;
        // Size in pixels
        uint32_t size;
    };

    // Renders an arrow from Vector3::zero to Vector3::forward that is transformed by world_transform
    void draw_arrow_3d(Arrow_3D, Matrix4 world_transform, Matrix4 view_projection_matrix, Vector2 viewport_size);
} // namespace anton_engine::gizmo

#endif // !EDITOR_LEVEL_EDITOR_GIZMO_ARROW_3D_HPP_INCLUDE
