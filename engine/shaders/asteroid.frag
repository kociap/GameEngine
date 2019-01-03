#version 450 core

in Frag_data {
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

uniform Material material;

out vec4 frag_color;

void main() {
    frag_color = texture(material.texture_diffuse0, fragment_data.texture_coordinates);
}
