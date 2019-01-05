#version 450 core

in vec2 texture_coordinates;
out vec4 frag_color;

uniform sampler2D scene_texture;
uniform float gamma;

void main() {
    frag_color.rgb = pow(texture(scene_texture, texture_coordinates).rgb, vec3(gamma));
}