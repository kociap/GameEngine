#pragma once

#include <anton/flat_hash_map.hpp>
#include <anton/math/mat4.hpp>
#include <anton/math/vec2.hpp>
#include <anton/math/vec3.hpp>
#include <anton/math/vec4.hpp>
#include <anton/string_view.hpp>
#include <anton/type_traits.hpp>
#include <core/color.hpp>
#include <core/types.hpp>
#include <shaders/shader_stage.hpp>

namespace anton_engine {
  class Shader {
  public:
    Shader(bool create = true);
    Shader(Shader const&) = delete;
    Shader& operator=(Shader const&) = delete;
    Shader(Shader&&) noexcept;
    Shader& operator=(Shader&&) noexcept;
    ~Shader();

    void attach(Shader_Stage const& shader);
    void link();
    void use();
    void detach(Shader_Stage const&);

    void set_int(anton::String_View, i32);
    void set_uint(anton::String_View, u32);
    void set_float(anton::String_View, f32);
    void set_vec2(anton::String_View, Vec2);
    void set_vec3(anton::String_View, Vec3);
    void set_vec3(anton::String_View, Color);
    void set_vec4(anton::String_View, Color);
    void set_mat4(anton::String_View, Mat4 const&);

    u32 get_shader_native_handle() const
    {
      return program;
    }

    friend void swap(Shader&, Shader&);
    friend void delete_shader(Shader&);

  private:
    anton::Flat_Hash_Map<u64, i32> uniform_cache;
    u32 program = 0;
  };

  template<typename... Ts>
  auto create_shader(Ts const&... shaders)
  {
    static_assert((... && anton::is_same<Shader_Stage, anton::decay<Ts>>),
                  "Passed arguments are not of Shader_Stage type");
    Shader shader;
    (shader.attach(shaders), ...);
    shader.link();
    (shader.detach(shaders), ...);
    return shader;
  }
} // namespace anton_engine
