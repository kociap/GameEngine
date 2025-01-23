#pragma once

#include <anton/string_view.hpp>
#include <core/types.hpp>
#include <rendering/opengl.hpp>

namespace anton_engine {
  class Shader;

  class Shader_Stage {
  public:
    Shader_Stage() = delete;
    Shader_Stage(Shader_Stage const&) = delete;
    Shader_Stage& operator=(Shader_Stage const&) = delete;

    Shader_Stage(anton::String_View name, opengl::Shader_Stage_Type type,
                 anton::String_View source);
    Shader_Stage(Shader_Stage&& shader) noexcept;
    Shader_Stage& operator=(Shader_Stage&& shader) noexcept;
    ~Shader_Stage();

  private:
    friend class Shader;

    opengl::Shader_Stage_Type type;
    u32 shader;
  };
} // namespace anton_engine
