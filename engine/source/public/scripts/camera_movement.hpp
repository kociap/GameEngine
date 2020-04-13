#ifndef SCRIPTS_CAMERA_MOVEMENT_HPP_INCLUDE
#define SCRIPTS_CAMERA_MOVEMENT_HPP_INCLUDE

#include <core/class_macros.hpp>
#include <core/math/vector3.hpp>
#include <core/serialization/serialization.hpp>
#include <engine/components/camera.hpp>
#include <engine/components/transform.hpp>

namespace anton_engine {
    class COMPONENT Camera_Movement {
    public:
        static void update(Camera_Movement&, Camera&, Transform&);

        Vector3 camera_side = Vector3{1.0f, 0.0f, 0.0f};
    };
} // namespace anton_engine

ANTON_DEFAULT_SERIALIZABLE(anton_engine::Camera_Movement)

#endif // !SCRIPTS_CAMERA_MOVEMENT_HPP_INCLUDE
