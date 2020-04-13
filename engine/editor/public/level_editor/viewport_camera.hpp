#ifndef EDITOR_LEVEL_EDITOR_VIEWPORT_CAMERA_HPP_INCLUDE
#define EDITOR_LEVEL_EDITOR_VIEWPORT_CAMERA_HPP_INCLUDE

#include <core/class_macros.hpp>
#include <core/math/vector3.hpp>
#include <core/serialization/serialization.hpp>
#include <core/types.hpp>

// TODO: Temporarily moved to public due to generated code not being able to include this header

namespace anton_engine {
    class Transform;

    class COMPONENT Viewport_Camera {
    public:
        static void update(Viewport_Camera&, Transform&);

        i32 viewport_index = 0;
        Vector3 camera_side = Vector3{1.0f, 0.0f, 0.0f};
    };
} // namespace anton_engine

ANTON_DEFAULT_SERIALIZABLE(anton_engine::Viewport_Camera)

#endif // !EDITOR_LEVEL_EDITOR_VIEWPORT_CAMERA_HPP_INCLUDE
