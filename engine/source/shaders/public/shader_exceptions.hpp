#ifndef SHADERS_EXCEPTIONS_HPP_INCLUDE
#define SHADERS_EXCEPTIONS_HPP_INCLUDE

#include <exception>
#include <stdexcept>

namespace anton_engine {
    class Shader_Not_Created: public std::runtime_error {
    public:
        using std::runtime_error::runtime_error;
    };

    class Shader_Compilation_Failed: public std::runtime_error {
    public:
        using std::runtime_error::runtime_error;
    };

    class Program_Not_Created: public std::runtime_error {
    public:
        using std::runtime_error::runtime_error;
    };

    class Program_Linking_Failed: public std::runtime_error {
    public:
        using std::runtime_error::runtime_error;
    };
} // namespace anton_engine

#endif // !SHADERS_EXCEPTIONS_HPP_INCLUDE
