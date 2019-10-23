#version 450 core

in vec2 tex_coords;
out vec4 frag_color;

layout(binding = 0) uniform sampler2D scene_texture;
uniform float gamma;

void main() {
    frag_color.rgb = pow(texture(scene_texture, tex_coords).rgb, vec3(gamma));
}