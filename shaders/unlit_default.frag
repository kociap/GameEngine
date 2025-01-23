#version 450 core

in Frag_Data {
    vec3 fragment_position;
    vec3 normal;
    vec2 texture_coordinates;
}
fragment_data;

struct Material {
    sampler2D texture_diffuse0;
    sampler2D texture_diffuse1;
    sampler2D texture_diffuse2;
    sampler2D texture_diffuse3;
    sampler2D texture_specular0;
    sampler2D texture_specular1;
    sampler2D texture_specular2;
    sampler2D texture_specular3;
};

struct Camera {
    vec3 position;
};

uniform Material material;
uniform Camera camera;

out vec4 frag_color;

void main() {
    frag_color = texture(material.texture_diffuse0, fragment_data.texture_coordinates);
}
