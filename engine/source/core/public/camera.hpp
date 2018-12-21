#ifndef CORE_CAMERA_HPP_INCLUDE
#define CORE_CAMERA_HPP_INCLUDE

#include "math/vector3.hpp"

class Camera {
public:
    Vector3 position = Vector3::zero;
    Vector3 front = Vector3::forward;
    Vector3 rotation = Vector3::zero;
    float fov = 70;
};

#endif // !CORE_CAMERA_HPP_INCLUDE
