#ifndef EDITOR_LEVEL_EDITOR_GIZMO_CONTEXT_HPP_INCLUDE
#define EDITOR_LEVEL_EDITOR_GIZMO_CONTEXT_HPP_INCLUDE

#include <components/transform.hpp>
#include <math/vector2.hpp>
#include <math/vector3.hpp>

namespace anton_engine {
    enum class Gizmo_Transform_Type {
        translate,
        rotate,
        scale,
    };

    enum class Gizmo_Transform_Space {
        world,
        local,
        // view,
        // gimbal,
    };

    struct Gizmo_Grab_Data {
        // Transform before applying gizmo transformations
        Transform cached_transform;
        Vector3 grabbed_axis;
        // Point in the world where mouse originally grabbed gizmo
        Vector3 mouse_grab_point;
        Vector3 plane_normal;
        float plane_distance;
        // Point on screen where mouse originally grabbed gizmo
        Vector2 screen_mouse_pos;
        bool grabbed;
    };

    struct Gizmo_Context {
        Gizmo_Transform_Type type;
        Gizmo_Transform_Space space;
        Gizmo_Grab_Data grab;
    };

    Gizmo_Context& get_gizmo_context();
} // namespace anton_engine

#endif // !EDITOR_LEVEL_EDITOR_GIZMO_CONTEXT_HPP_INCLUDE
