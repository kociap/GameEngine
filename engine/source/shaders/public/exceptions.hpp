#ifndef SHADERS_EXCEPTIONS_HPP_INCLUDE
#define SHADERS_EXCEPTIONS_HPP_INCLUDE

#include <exception>
#include <stdexcept>

class Shader_not_created: public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

class Shader_compilation_failed : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

class Program_not_created : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

class Program_linking_failed : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

#endif // !SHADERS_EXCEPTIONS_HPP_INCLUDE