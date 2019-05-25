#ifndef EDITOR_LEVEL_EDITOR_GIZMO_HPP_INCLUDE
#define EDITOR_LEVEL_EDITOR_GIZMO_HPP_INCLUDE

#include "color.hpp"
#include "math/vector3.hpp"

namespace gizmo {
    void draw_line(Vector3 start, Vector3 end, Color, float time = 0.0f);
    // Size in meters
    void draw_point(Vector3 position, float size, Color, float time = 0.0f);
    // Size in pixels
    void draw_fixed_size_point(Vector3 position, float size, Color, float time = 0.0f);
} // namespace gizmo

#endif // !EDITOR_LEVEL_EDITOR_GIZMO_HPP_INCLUDE
