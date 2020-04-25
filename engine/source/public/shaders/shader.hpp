#ifndef SHADERS_SHADER_HPP_INCLUDE
#define SHADERS_SHADER_HPP_INCLUDE

#include <core/atl/flat_hash_map.hpp>
#include <core/atl/string_view.hpp>
#include <core/atl/type_traits.hpp>
#include <core/math/vector2.hpp>
#include <core/math/vector3.hpp>
#include <core/types.hpp>

namespace anton_engine {
    class Vector3;
    class Color;
    class Matrix4;
    class Shader_Stage;

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

        void set_int(atl::String_View, i32);
        void set_uint(atl::String_View, u32);
        void set_float(atl::String_View, f32);
        void set_vec2(atl::String_View, Vector2);
        void set_vec3(atl::String_View, Vector3);
        void set_vec3(atl::String_View, Color);
        void set_vec4(atl::String_View, Color);
        void set_matrix4(atl::String_View, Matrix4 const&);

        u32 get_shader_native_handle() const {
            return program;
        }

        friend void swap(Shader&, Shader&);
        friend void delete_shader(Shader&);

    private:
        atl::Flat_Hash_Map<u64, i32> uniform_cache;
        u32 program = 0;
    };

    template<typename... Ts>
    auto create_shader(Ts const&... shaders) {
        static_assert((... && atl::is_same<Shader_Stage, atl::decay<Ts>>), "Passed arguments are not of Shader_Stage type");
        Shader shader;
        (shader.attach(shaders), ...);
        shader.link();
        (shader.detach(shaders), ...);
        return shader;
    }
} // namespace anton_engine

#endif // !SHADERS_SHADER_HPP_INCLUDE
