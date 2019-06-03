#ifndef LEVEL_EDITOR_LEVEL_EDITOR_HPP_INCLUDE
#define LEVEL_EDITOR_LEVEL_EDITOR_HPP_INCLUDE

#include "components/transform.hpp"
#include "math/vector3.hpp"

class Framebuffer;

class Level_Editor {
public:
    Level_Editor();
    ~Level_Editor();

    void prepare_editor_ui();

    Transform cached_gizmo_transform;
    Vector3 gizmo_mouse_grab_point;
    Vector3 gizmo_grabbed_axis;
    Framebuffer* framebuffer = nullptr;
    bool gizmo_grabbed = false;
};

#endif // !LEVEL_EDITOR_LEVEL_EDITOR_HPP_INCLUDE
