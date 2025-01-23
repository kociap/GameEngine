#pragma once

#include <core/class_macros.hpp>
#include <core/color.hpp>
#include <core/serialization/serialization.hpp>

namespace anton_engine {
  class COMPONENT Point_Light_Component {
  public:
    Color color = Color::white;
    f32 intensity = 1.0f;
    bool dynamic = false;
  };
} // namespace anton_engine

ANTON_DEFAULT_SERIALIZABLE(::anton_engine::Point_Light_Component)
