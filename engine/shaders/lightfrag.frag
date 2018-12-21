#version 450 core

out vec4 frag_color;

uniform vec3 light_color;

void main() {
    // frag_color = texture(frag_texture1, vertex_texture_coord) * vertex_color;
    // frag_color = mix(texture(frag_texture0, vertex_texture_coord), texture(frag_texture1, vec2(-vertex_texture_coord.x, vertex_texture_coord.y)), 0.5);
    frag_color = vec4(light_color, 1.0);
}