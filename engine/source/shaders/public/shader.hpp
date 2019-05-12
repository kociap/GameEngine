#ifndef SHADERS_SHADER_HPP_INCLUDE
#define SHADERS_SHADER_HPP_INCLUDE

#include "object.hpp"
#include <string>
#include <type_traits>

class Vector3;
class Color;
class Matrix4;
class Shader_File;

class Shader: public Object {
public:
    Shader();
    Shader(Shader const&) = delete;
    Shader& operator=(Shader const&) = delete;
    Shader(Shader&&) noexcept;
    Shader& operator=(Shader&&) noexcept;
    ~Shader();

    void attach(Shader_File const& shader);
    void link();
    void use();
    void detach(Shader_File const&);

    void set_int(std::string const&, int);
    void set_int(char const*, int);
    void set_float(std::string const&, float);
    void set_float(char const*, float);
    void set_vec3(std::string const&, Vector3 const&);
    void set_vec3(char const*, Vector3 const&);
    void set_vec3(std::string const&, Color const&);
    void set_vec3(char const*, Color const&);
    void set_matrix4(std::string const&, Matrix4 const&);
    void set_matrix4(char const*, Matrix4 const&);

    friend void swap(Shader&, Shader&);

    uint32_t program = 0;
};

template <typename... Ts>
// Return type causes internal compiler error on msvc
// auto create_shader(Ts const&... shaders) -> std::enable_if_t<(... && std::is_same_v<Shader_File, std::decay_t<Ts>>), Shader> {
auto create_shader(Ts const&... shaders) {
    static_assert((... && std::is_same_v<Shader_File, std::decay_t<Ts>>), "Passed arguments are not of Shader_File type");
    Shader shader;
    (shader.attach(shaders), ...);
    shader.link();
    (shader.detach(shaders), ...);
    return shader;
}

#endif // !SHADERS_SHADER_HPP_INCLUDE