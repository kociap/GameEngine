#version 450 core

in Frag_Data {
    vec3 color;
} frag_data;

out vec4 frag_color;

void main() {
    frag_color = vec4(frag_data.color, 1.0);
}