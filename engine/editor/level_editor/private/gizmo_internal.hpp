#ifndef EDITOR_LEVEL_EDITOR_GIZMO_INTERNAL_HPP_INCLUDE
#define EDITOR_LEVEL_EDITOR_GIZMO_INTERNAL_HPP_INCLUDE

#include <math/matrix4.hpp>
#include <math/vector3.hpp>

namespace anton_engine::gizmo {
    void init();
    void terminate();
    void update();
    void draw(Vector3 camera_position, Matrix4 view, Matrix4 projection);
} // namespace anton_engine::gizmo

#endif // !EDITOR_LEVEL_EDITOR_GIZMO_INTERNAL_HPP_INCLUDE
