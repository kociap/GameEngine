#ifndef SHADERS_PROGRAM_HPP_INCLUDE
#define SHADERS_PROGRAM_HPP_INCLUDE

#include "glad/glad.h"
#include "shader.hpp"
#include <string>

class Program {
public:
    Program();
    ~Program();

    void attach(Shader const& shader);
    void link();
    void use();
    void detach(Shader const&);

    GLint get_uniform(std::string const&);
    GLint get_uniform(char const*);

private:
    GLuint program;
};

#endif // !SHADERS_PROGRAM_HPP_INCLUDE