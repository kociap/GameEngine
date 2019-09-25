#ifndef EDITOR_GIZMO_DIAL_3D_HPP_INCLUDE
#define EDITOR_GIZMO_DIAL_3D_HPP_INCLUDE

#include <color.hpp>
#include <math/matrix4.hpp>
#include <math/vector2.hpp>
#include <math/vector3.hpp>

namespace anton_engine::gizmo {
    struct Dial_3D {
        Color color;
        // size in pixels
        uint32_t size;
    };

    void draw_dial_3d(Dial_3D, Matrix4 world_transform, Matrix4 view_mat, Matrix4 projection_mat, Vector3 camera_pos, Vector2 viewport_size);
} // namespace anton_engine::gizmo

#endif // !EDITOR_GIZMO_DIAL_3D_HPP_INCLUDE
