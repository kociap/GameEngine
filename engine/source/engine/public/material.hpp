#ifndef ENGINE_COMPONENTS_MATERIAL_HPP_INCLUDE
#define ENGINE_COMPONENTS_MATERIAL_HPP_INCLUDE

#include <cstdint>

namespace anton_engine {
    class Texture {
    public:
        uint32_t handle = 0;
    };

    class Material {
    public:
        Texture diffuse_texture;
        Texture specular_texture;
        Texture normal_map;
        float shininess = 32.0f;
    };
} // namespace anton_engine

#endif // !ENGINE_COMPONENTS_MATERIAL_HPP_INCLUDE
