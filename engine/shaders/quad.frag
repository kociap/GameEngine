#version 450 core

in vec2 texture_coordinates;

uniform sampler2D scene_texture;

out vec4 frag_color;

void main() {
    frag_color.rgb = vec3(texture(scene_texture, texture_coordinates).r);
}