#version 450 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 color;

out vec4 vertex_color;
out vec4 vertex_pos;

void main() {
    gl_Position = vec4(pos, 1.0);
    vertex_color = vec4(color, 1.0);
    vertex_pos = vec4(pos, 1.0);
}
