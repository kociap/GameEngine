#pragma once

#include <anton/math/vec3.hpp>
#include <core/class_macros.hpp>
#include <core/color.hpp>
#include <core/serialization/serialization.hpp>

namespace anton_engine {
  // By default points down along y axis.
  //
  class COMPONENT Spot_Light_Component {
  public:
    Vec3 direction = Vec3{0.0f, -1.0f, 0.0f};
    Color color = Color::white;
    f32 cutoff_angle = 25.0f;
    f32 blend_angle = 20.0f;
    f32 intensity = 1.0f;
    bool dynamic = false;
  };
} // namespace anton_engine

ANTON_DEFAULT_SERIALIZABLE(anton_engine::Spot_Light_Component)
