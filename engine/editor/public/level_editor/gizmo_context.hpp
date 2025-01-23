#pragma once

#include <anton/math/vec2.hpp>
#include <anton/math/vec3.hpp>
#include <engine/components/transform.hpp>

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
    Vec3 grabbed_axis;
    // Point in the world where mouse originally grabbed gizmo
    Vec3 mouse_grab_point;
    Vec3 plane_normal;
    float plane_distance;
    // Point on screen where mouse originally grabbed gizmo
    Vec2 screen_mouse_pos;
    bool grabbed;
  };

  struct Gizmo_Context {
    Gizmo_Transform_Type type;
    Gizmo_Transform_Space space;
    Gizmo_Grab_Data grab;
  };

  Gizmo_Context& get_gizmo_context();
} // namespace anton_engine
