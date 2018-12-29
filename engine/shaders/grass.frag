#version 450 core

in vec2 texture_coordinates;

struct Material {
    sampler2D texture_diffuse0;
};

uniform Material material;

out vec4 frag_color;

void main() {
    frag_color = texture(material.texture_diffuse0, texture_coordinates);
    if (frag_color.a < 0.1) {
        discard;
    }
}