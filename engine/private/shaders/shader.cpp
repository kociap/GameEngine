#include <shaders/shader.hpp>

#include <rendering/glad.hpp>

#include <anton/array.hpp>
#include <anton/math/mat4.hpp>
#include <anton/math/vec3.hpp>
#include <anton/utility.hpp>
#include <core/color.hpp>
#include <rendering/opengl.hpp>
#include <shaders/shader_exceptions.hpp>
#include <shaders/shader_stage.hpp>

namespace anton_engine {
  Shader::Shader(bool create)
  {
    if(create) {
      program = glCreateProgram();
      if(program == 0) {
        throw Program_Not_Created("");
      }
    }
  }

  Shader::Shader(Shader&& other) noexcept
    : uniform_cache(ANTON_MOV(other.uniform_cache))
  {
    anton::swap(other.program, program);
  }

  Shader& Shader::operator=(Shader&& other) noexcept
  {
    anton::swap(other.program, program);
    anton::swap(uniform_cache, other.uniform_cache);
    return *this;
  }

  Shader::~Shader()
  {
    if(program != 0) {
      glDeleteProgram(program);
    }
  }

  void Shader::attach(Shader_Stage const& shader)
  {
    glAttachShader(program, shader.shader);
  }

  static void
  build_shader_uniform_cache(u32 program,
                             anton::Flat_Hash_Map<u64, i32>& uniform_cache)
  {
    i32 active_uniforms;
    glGetProgramInterfaceiv(program, GL_UNIFORM, GL_ACTIVE_RESOURCES,
                            &active_uniforms);
    // glGetProgramInterfaceiv outputs the max name length including null-terminator.
    i32 uniform_max_name_length;
    glGetProgramInterfaceiv(program, GL_UNIFORM, GL_MAX_NAME_LENGTH,
                            &uniform_max_name_length);
    anton::Array<char> name(uniform_max_name_length);
    for(i32 uniform_index = 0; uniform_index < active_uniforms;
        ++uniform_index) {
      // glGetActiveUniformName outputs the number of characters written not including null-terminator.
      i32 name_length;
      glGetActiveUniformName(program, static_cast<u32>(uniform_index),
                             uniform_max_name_length, &name_length, &name[0]);
      i32 const location = glGetUniformLocation(program, &name[0]);
      anton::String_View const name_str =
        anton::String_View(&name[0], name_length);
      uniform_cache.emplace(anton::hash(name_str), location);
    }
  }

  void Shader::link()
  {
    glLinkProgram(program);
    GLint link_status;
    glGetProgramiv(program, GL_LINK_STATUS, &link_status);
    if(link_status == GL_FALSE) {
      GLint log_length;
      glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);
      anton::Array<GLchar> log{log_length};
      glGetProgramInfoLog(program, log_length, &log_length, &log[0]);
      throw Program_Linking_Failed(anton::String_View{log.data(), log.size()});
    }

    build_shader_uniform_cache(program, uniform_cache);
  }

  void Shader::use()
  {
    glUseProgram(program);
  }

  void Shader::detach(Shader_Stage const& shader)
  {
    glDetachShader(program, shader.shader);
  }

  void Shader::set_int(anton::String_View const name, i32 const a)
  {
    u64 const h = anton::hash(name);
    auto iter = uniform_cache.find(h);
    if(iter != uniform_cache.end()) {
      glUniform1i(iter->value, a);
    }
  }

  void Shader::set_uint(anton::String_View const name, u32 const a)
  {
    u64 const h = anton::hash(name);
    auto iter = uniform_cache.find(h);
    if(iter != uniform_cache.end()) {
      glUniform1ui(iter->value, a);
    }
  }

  void Shader::set_float(anton::String_View const name, float const a)
  {
    u64 const h = anton::hash(name);
    auto iter = uniform_cache.find(h);
    if(iter != uniform_cache.end()) {
      glUniform1f(iter->value, a);
    }
  }

  void Shader::set_vec2(anton::String_View const name, Vec2 const vec)
  {
    u64 const h = anton::hash(name);
    auto iter = uniform_cache.find(h);
    if(iter != uniform_cache.end()) {
      glUniform2fv(iter->value, 1, &vec.x);
    }
  }

  void Shader::set_vec3(anton::String_View const name, Vec3 const vec)
  {
    u64 const h = anton::hash(name);
    auto iter = uniform_cache.find(h);
    if(iter != uniform_cache.end()) {
      glUniform3fv(iter->value, 1, &vec.x);
    }
  }

  void Shader::set_vec3(anton::String_View const name, Color const c)
  {
    u64 const h = anton::hash(name);
    auto iter = uniform_cache.find(h);
    if(iter != uniform_cache.end()) {
      glUniform3fv(iter->value, 1, &c.r);
    }
  }

  void Shader::set_vec4(anton::String_View const name, Color const c)
  {
    u64 const h = anton::hash(name);
    auto iter = uniform_cache.find(h);
    if(iter != uniform_cache.end()) {
      glUniform4fv(iter->value, 1, &c.r);
    }
  }

  void Shader::set_mat4(anton::String_View const name, Mat4 const& mat)
  {
    u64 const h = anton::hash(name);
    auto iter = uniform_cache.find(h);
    if(iter != uniform_cache.end()) {
      glUniformMatrix4fv(iter->value, 1, GL_FALSE, mat.data());
    }
  }

  void swap(Shader& s1, Shader& s2)
  {
    anton::swap(s1.program, s2.program);
    anton::swap(s1.uniform_cache, s2.uniform_cache);
  }

  void delete_shader(Shader& shader)
  {
    if(shader.program != 0) {
      glDeleteProgram(shader.program);
      shader.program = 0;
    }
  }
} // namespace anton_engine
