#ifndef SCRIPTS_CAMERA_MOVEMENT_HPP_INCLUDE
#define SCRIPTS_CAMERA_MOVEMENT_HPP_INCLUDE

#include <class_macros.hpp>
#include <components/camera.hpp>
#include <components/transform.hpp>
#include <math/vector3.hpp>
#include <serialization/serialization.hpp>

namespace anton_engine {
    class COMPONENT Camera_Movement {
    public:
        static void update(Camera_Movement&, Camera&, Transform&);

        Vector3 camera_side = Vector3::right;
    };
} // namespace anton_engine

ANTON_DEFAULT_SERIALIZABLE(anton_engine::Camera_Movement);
ANTON_DEFAULT_DESERIALIZABLE(anton_engine::Camera_Movement);

#endif // !SCRIPTS_CAMERA_MOVEMENT_HPP_INCLUDE
