#ifndef SCRIPTS_CAMERA_MOVEMENT_HPP_INCLUDE
#define SCRIPTS_CAMERA_MOVEMENT_HPP_INCLUDE

#include "components/camera.hpp"
#include "components/transform.hpp"
#include "math/vector3.hpp"
#include "serialization.hpp"

class Camera_Movement {
public:
    static void update(Camera_Movement&, Camera&, Transform&);

    Vector3 camera_side = Vector3::right;
};

template <>
struct serialization::use_default_deserialize<Camera_Movement>: std::true_type {};

#endif // !SCRIPTS_CAMERA_MOVEMENT_HPP_INCLUDE
