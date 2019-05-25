#ifndef EDITOR_LEVEL_EDITOR_GIZMO_INTERNAL_HPP_INCLUDE
#define EDITOR_LEVEL_EDITOR_GIZMO_INTERNAL_HPP_INCLUDE

#include "math/matrix4.hpp"
#include "math/vector3.hpp"

namespace gizmo {
    void init();
    void terminate();
    void update();
    void draw(Vector3 camera_position, Matrix4 view, Matrix4 projection);
} // namespace gizmo

#endif // !EDITOR_LEVEL_EDITOR_GIZMO_INTERNAL_HPP_INCLUDE
