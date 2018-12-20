#version 450 core

in vec2 vertex_texture_coord;

out vec4 frag_color;

uniform sampler2D frag_texture0;
uniform sampler2D frag_texture1;

void main() {
    // frag_color = texture(frag_texture1, vertex_texture_coord) * vertex_color;
    frag_color = mix(texture(frag_texture0, vertex_texture_coord), texture(frag_texture1, vec2(-vertex_texture_coord.x, vertex_texture_coord.y)), 0.5);
}