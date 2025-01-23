#pragma once

#include <shaders/shader.hpp>

namespace anton_engine {
  enum class Builtin_Editor_Shader {
    outline_mix = 0,
    grid = 1,
    imgui = 2,
  };

  Shader& get_builtin_shader(Builtin_Editor_Shader);
} // namespace anton_engine
