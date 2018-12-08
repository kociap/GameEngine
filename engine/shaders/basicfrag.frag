#version 450 core

in vec4 vertex_color;
in vec4 vertex_pos;

out vec4 frag_color;

void main() {
    frag_color = vertex_pos;
}