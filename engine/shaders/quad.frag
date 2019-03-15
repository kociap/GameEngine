#version 450 core

in vec2 tex_coords;

uniform sampler2D scene_texture;

out vec4 frag_color;

void main() {
    frag_color.rgb = vec3(texture(scene_texture, tex_coords).r);
}