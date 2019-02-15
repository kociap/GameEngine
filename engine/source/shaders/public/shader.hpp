#ifndef SHADERS_SHADER_HPP_INCLUDE
#define SHADERS_SHADER_HPP_INCLUDE

#include "glad/glad.h"
#include "object.hpp"
#include <string>

class Vector3;
class Color;
class Matrix4;
class Shader_File;

class Shader : public Object {
public:
    Shader(bool create_program = true);
    Shader(Shader const&) = delete;
    Shader& operator=(Shader const&) = delete;
    Shader(Shader&&) noexcept;
    Shader& operator=(Shader&&) noexcept;
    ~Shader();

    void create();
    void attach(Shader_File const& shader);
    void link();
    void use();
    void detach(Shader_File const&);

    GLint get_uniform(std::string const&);
    GLint get_uniform(char const*);

    void set_int(std::string const&, int);
    void set_float(std::string const&, float);
    void set_vec3(std::string const&, Vector3 const&);
    void set_vec3(std::string const&, Color const&);
    void set_matrix4(std::string const&, Matrix4 const&);

private:
    GLuint program = 0;
};

#endif // !SHADERS_SHADER_HPP_INCLUDE