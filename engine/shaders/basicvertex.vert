#version 450 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 color;
layout(location = 2) in vec2 texture_coord;

out vec4 vertex_color;
out vec2 vertex_texture_coord;

void main() {
    gl_Position = vec4(pos, 1.0);
    vertex_color = vec4(color, 1.0);
    vertex_texture_coord = texture_coord;
}
