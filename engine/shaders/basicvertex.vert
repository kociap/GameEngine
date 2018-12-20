#version 450 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 texture_coord;

out vec2 vertex_texture_coord;

uniform mat4 transform;

void main() {
    gl_Position = transform * vec4(pos, 1.0);
    vertex_texture_coord = texture_coord;
}
