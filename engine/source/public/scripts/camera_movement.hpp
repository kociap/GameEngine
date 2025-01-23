#pragma once

#include <anton/math/vec3.hpp>
#include <core/class_macros.hpp>
#include <core/serialization/serialization.hpp>
#include <engine/components/camera.hpp>
#include <engine/components/transform.hpp>

namespace anton_engine {
  class COMPONENT Camera_Movement {
  public:
    static void update(Camera_Movement&, Camera&, Transform&);

    Vec3 camera_side = Vec3{1.0f, 0.0f, 0.0f};
  };
} // namespace anton_engine

ANTON_DEFAULT_SERIALIZABLE(anton_engine::Camera_Movement)
