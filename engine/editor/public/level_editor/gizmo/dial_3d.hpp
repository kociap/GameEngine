#ifndef EDITOR_GIZMO_DIAL_3D_HPP_INCLUDE
#define EDITOR_GIZMO_DIAL_3D_HPP_INCLUDE

#include <core/color.hpp>
#include <core/math/matrix4.hpp>
#include <core/math/vector2.hpp>
#include <core/math/vector3.hpp>

namespace anton_engine::gizmo {
    struct Dial_3D {
        Color color;
        // size in pixels
        uint32_t size;
    };

    void draw_dial_3d(Dial_3D, Matrix4 world_transform, Matrix4 view_projection, Vector3 camera_pos, Vector2 viewport_size);
    // Tests the arrow for intersection and returns the distance to the intersection point along the ray if the ray intersects the bounding volumes.
    atl::Optional<float> intersect_dial_3d(Ray, Dial_3D, Matrix4 world_transform, Matrix4 view_projection_matrix, Vector2 viewport_size);
} // namespace anton_engine::gizmo

#endif // !EDITOR_GIZMO_DIAL_3D_HPP_INCLUDE
