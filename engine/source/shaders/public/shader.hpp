#ifndef SHADERS_SHADER_HPP_INCLUDE
#define SHADERS_SHADER_HPP_INCLUDE

#include "glad/glad.h"
#include "math/matrix4.hpp"
#include "shaderfile.hpp"
#include <string>

class Shader {
public:
    Shader();
    ~Shader();

    void attach(Shader_file const& shader);
    void link();
    void use();
    void detach(Shader_file const&);

    GLint get_uniform(std::string const&);
    GLint get_uniform(char const*);

    void setMatrix4(std::string const&, Matrix4 const&);

private:
    GLuint program;
};

#endif // !SHADERS_SHADER_HPP_INCLUDE