#ifndef SHADERS_SHADER_HPP_INCLUDE
#define SHADERS_SHADER_HPP_INCLUDE

#include <anton_stl/type_traits.hpp>
#include <string>
#include <unordered_map>

namespace anton_engine {
    class Vector3;
    class Color;
    class Matrix4;
    class Shader_File;

    class Shader {
    public:
        Shader(bool create = true);
        Shader(Shader const&) = delete;
        Shader& operator=(Shader const&) = delete;
        Shader(Shader&&) noexcept;
        Shader& operator=(Shader&&) noexcept;
        ~Shader();

        void attach(Shader_File const& shader);
        void link();
        void use();
        void detach(Shader_File const&);

        // TODO: String_View
        void set_int(std::string const&, int);
        void set_int(char const*, int);
        void set_float(std::string const&, float);
        void set_float(char const*, float);
        void set_vec3(std::string const&, Vector3);
        void set_vec3(char const*, Vector3);
        void set_vec3(std::string const&, Color);
        void set_vec3(char const*, Color);
        void set_vec4(std::string const&, Color);
        void set_vec4(char const*, Color);
        void set_matrix4(std::string const&, Matrix4 const&);
        void set_matrix4(char const*, Matrix4 const&);

        uint32_t get_shader_native_handle() const {
            return program;
        }

        friend void swap(Shader&, Shader&);
        friend void delete_shader(Shader&);

    private:
        std::unordered_map<std::string, int32_t> uniform_cache;
        uint32_t program = 0;
    };

    template <typename... Ts>
    auto create_shader(Ts const&... shaders) {
        static_assert((... && anton_stl::is_same<Shader_File, anton_stl::decay<Ts>>), "Passed arguments are not of Shader_File type");
        Shader shader;
        (shader.attach(shaders), ...);
        shader.link();
        (shader.detach(shaders), ...);
        return shader;
    }
} // namespace anton_engine

#endif // !SHADERS_SHADER_HPP_INCLUDE
