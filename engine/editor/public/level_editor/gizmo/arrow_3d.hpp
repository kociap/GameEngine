#ifndef EDITOR_LEVEL_EDITOR_GIZMO_ARROW_3D_HPP_INCLUDE
#define EDITOR_LEVEL_EDITOR_GIZMO_ARROW_3D_HPP_INCLUDE

#include <core/types.hpp>
#include <core/stl/optional.hpp>
#include <core/color.hpp>
#include <level_editor/gizmo/gizmo.hpp>
#include <core/math/matrix4.hpp>
#include <core/math/vector2.hpp>
#include <physics/ray.hpp>

namespace anton_engine::gizmo {
    // Arrow head style
    enum class Arrow_3D_Style {
        // none
        cone,
        cube,
    };

    struct Arrow_3D {
        Arrow_3D_Style draw_style;
        Color color;
        // Size in pixels
        u32 size;
    };

    // Renders an arrow from Vector3::zero to Vector3::forward that is transformed by world_transform.
    void draw_arrow_3d(Arrow_3D, Matrix4 world_transform, Matrix4 view_projection_matrix, Vector2 viewport_size, Vector3 camera_pos);
    // Tests the arrow for intersection and returns the distance to the intersection point along the ray if the ray intersects the bounding volumes.
    anton_stl::Optional<float> intersect_arrow_3d(Ray, Arrow_3D, Matrix4 world_transform, Matrix4 view_projection_matrix, Vector2 viewport_size);
} // namespace anton_engine::gizmo

#endif // !EDITOR_LEVEL_EDITOR_GIZMO_ARROW_3D_HPP_INCLUDE
