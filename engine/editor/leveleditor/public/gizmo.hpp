#ifndef EDITOR_LEVEL_EDITOR_GIZMO_HPP_INCLUDE
#define EDITOR_LEVEL_EDITOR_GIZMO_HPP_INCLUDE

#include "color.hpp"
#include "containers/vector.hpp"
#include "math/vector3.hpp"

namespace gizmo {
    struct Vertex {
        Vector3 position;
        Color color;
    };

    // Size in meters
    void draw_point(Vector3 position, float size, Color, float time = 0.0f, bool depth_test = true);
    void draw_line(Vector3 start, Vector3 end, Color, float time = 0.0f, bool depth_test = true);
    void draw_polyline(containers::Vector<Vertex> vertices, bool closed, float time = 0.0f, bool depth_test = true);
    void draw_triangle(Vector3 vert1, Vector3 vert2, Vector3 vert3, Color, float time = 0.0f, bool depth_test = true);
    void draw_cone(Vector3 position, Vector3 normal, float base_radius, uint32_t base_point_count, float height, Color, float time = 0.0f,
                   bool depth_test = true);
    void draw_cube(Vector3 position, float size, Color, float time = 0.0f, bool depth_test = true);
    // Always filled
    void draw_polygon(containers::Vector<Vertex> vertices, float time = 0.0f, bool depth_test = true);
    void draw_circle(Vector3 position, Vector3 normal, float radius, Color, uint32_t point_count, bool fill, float time = 0.0f, bool depth_test = true);
} // namespace gizmo

#endif // !EDITOR_LEVEL_EDITOR_GIZMO_HPP_INCLUDE
