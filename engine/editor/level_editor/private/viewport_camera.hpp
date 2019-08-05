#ifndef EDITOR_LEVEL_EDITOR_VIEWPORT_CAMERA_HPP_INCLUDE
#define EDITOR_LEVEL_EDITOR_VIEWPORT_CAMERA_HPP_INCLUDE

#include <class_macros.hpp>
#include <math/vector3.hpp>
#include <serialization/serialization.hpp>

class Transform;

class COMPONENT Viewport_Camera {
public:
    static void update(Viewport_Camera&, Transform&);

    uint8_t viewport_index;
    Vector3 camera_side = Vector3::right;
};

DEFAULT_SERIALIZABLE(Viewport_Camera);
DEFAULT_DESERIALIZABLE(Viewport_Camera);

#endif // !EDITOR_LEVEL_EDITOR_VIEWPORT_CAMERA_HPP_INCLUDE
