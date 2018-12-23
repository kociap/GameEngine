#version 450 core

out vec4 frag_color;

uniform vec3 light_color;

void main() {
    frag_color = vec4(light_color, 1.0);
}