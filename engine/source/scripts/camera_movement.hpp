#ifndef SCRIPTS_CAMERA_MOVEMENT_HPP_INCLUDE
#define SCRIPTS_CAMERA_MOVEMENT_HPP_INCLUDE

#include <class_macros.hpp>
#include <components/camera.hpp>
#include <components/transform.hpp>
#include <math/vector3.hpp>
#include <serialization/serialization.hpp>

class COMPONENT Camera_Movement {
public:
    static void update(Camera_Movement&, Camera&, Transform&);

    Vector3 camera_side = Vector3::right;
};

DEFAULT_SERIALIZABLE(Camera_Movement);
DEFAULT_DESERIALIZABLE(Camera_Movement);

#endif // !SCRIPTS_CAMERA_MOVEMENT_HPP_INCLUDE
