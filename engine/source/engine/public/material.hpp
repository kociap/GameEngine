#ifndef ENGINE_COMPONENTS_MATERIAL_HPP_INCLUDE
#define ENGINE_COMPONENTS_MATERIAL_HPP_INCLUDE

#include <anton_int.hpp>

namespace anton_engine {
    // std140 requires a member structure be aligned to a multiple of vec4's alignment (16 in our case).
    // Textures are stored as array textures on gpu. index is rendering specific internal index that will be mapped
    //   to gl texture handle when the texture is bound. layer is the array texture layer.
    class alignas(16) Texture {
    public:
        static Texture const default_black;
        static Texture const default_black_transparent;
        static Texture const default_normal_map;

        u32 index = 0;
        float layer = 0;
    };

    inline Texture const Texture::default_black{0, 0.0f};
    inline Texture const Texture::default_black_transparent{0, 1.0f};
    inline Texture const Texture::default_normal_map{0, 2.0f};

    class Material {
    public:
        Texture diffuse_texture;
        Texture specular_texture;
        Texture normal_map;
        float shininess = 32.0f; // TODO: Remove
    };
} // namespace anton_engine

#endif // !ENGINE_COMPONENTS_MATERIAL_HPP_INCLUDE
