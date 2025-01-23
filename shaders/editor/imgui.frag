#version 450 core

in vec4 color;

uniform bool texture_bound;
layout(binding = 0) uniform sampler2D tex;

in Frag_Data {
    vec4 color;
    vec2 uv;
} fs_in;

out vec4 frag_color;

void main() {
    if(texture_bound) {
        frag_color = texture(tex, fs_in.uv);
    } else {
        frag_color = fs_in.color;
    }
}
