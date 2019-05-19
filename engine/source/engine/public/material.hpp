#ifndef ENGINE_COMPONENTS_MATERIAL_HPP_INCLUDE
#define ENGINE_COMPONENTS_MATERIAL_HPP_INCLUDE

#include <cstdint>

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

#endif // !ENGINE_COMPONENTS_MATERIAL_HPP_INCLUDE
