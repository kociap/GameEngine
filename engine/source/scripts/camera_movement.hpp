#ifndef SCRIPTS_CAMERA_MOVEMENT_HPP_INCLUDE
#define SCRIPTS_CAMERA_MOVEMENT_HPP_INCLUDE

#include "math/vector3.hpp"
#include "components/camera.hpp"
#include "components/transform.hpp"

class Camera_Movement {
public:
    static void update(Camera_Movement&, Camera&, Transform&);

    Vector3 camera_side = Vector3::right;
};

#endif // !SCRIPTS_CAMERA_MOVEMENT_HPP_INCLUDE
