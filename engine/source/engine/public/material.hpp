#ifndef ENGINE_COMPONENTS_MATERIAL_HPP_INCLUDE
#define ENGINE_COMPONENTS_MATERIAL_HPP_INCLUDE

#include <cstdint>

namespace anton_engine {
    // std140 requires a member structure be aligned to a multiple of vec4's alignment (16 in our case).
    class alignas(16) Texture {
    public:
        uint32_t index = 0;
        float layer = 0;
    };

    class Material {
    public:
        Texture diffuse_texture;
        Texture specular_texture;
        Texture normal_map;
        float shininess = 32.0f; // TODO: Remove
    };
} // namespace anton_engine

#endif // !ENGINE_COMPONENTS_MATERIAL_HPP_INCLUDE
